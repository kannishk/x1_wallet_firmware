#include "cy_factory_reset.h"
#include "controller_tap_cards.h"
#include "ui_instruction.h"

static uint8_t acceptable_cards = 15;
static uint16_t card1_len, card2_len;
static uint8_t card1[300], card2[300];
static uint8_t wallets_check = 0;
static void tap_card_backend(uint8_t *recv_apdu, uint16_t *recv_len);
static void decode_wallet_info_list(uint8_t *wallet_info[MAX_WALLETS_ALLOWED][2], uint8_t *data, uint16_t len);
static void get_common_wallet_list(uint8_t *wallet_info[MAX_WALLETS_ALLOWED][2]);

uint8_t factory_reset_mismatch_wallet_index = 0;

void cyc_factory_reset() {
    switch (flow_level.level_three) {
        case FACTORY_RESET_INIT:
            wallets_check = 0;
            for (int i = 0; i < get_wallet_count(); i++) {
                // Cards needed only if any valid wallets exist
                if (get_wallet_state(i) == VALID_WALLET)
                    wallets_check = 1;
            }
            flow_level.level_three = wallets_check ? FACTORY_RESET_INFO : FACTORY_RESET_CONFIRM;
            break;

        case FACTORY_RESET_INFO:
            flow_level.level_three = FACTORY_RESET_CONFIRM;
            break;

        case FACTORY_RESET_CONFIRM:
            acceptable_cards = 15;
            card1_len = 0;
            card2_len = 0;
            memzero(card1, sizeof(card1));
            memzero(card2, sizeof(card2));
            tap_card_data.retries = 5;
            flow_level.level_three = wallets_check ? FACTORY_RESET_TAP_CARD1 : FACTORY_RESET_ERASING;
            break;

        case FACTORY_RESET_TAP_CARD1:
            tap_card_data.lvl4_retry_point = 1;
            tap_card_data.lvl3_retry_point = FACTORY_RESET_TAP_CARD1;
            tap_card_backend(card1, &card1_len);
            ASSERT(card1_len < sizeof(card1));
            break;

        case FACTORY_RESET_TAP_CARD2:
            tap_card_data.lvl4_retry_point = 1;
            tap_card_data.lvl3_retry_point = FACTORY_RESET_TAP_CARD2;
            tap_card_backend(card2, &card2_len);
            ASSERT(card2_len < sizeof(card2));
            break;

        case FACTORY_RESET_CHECK: {
            const uint8_t *wallet_name, *wallet_id;
            uint8_t *wallet_info[MAX_WALLETS_ALLOWED][2];
            uint8_t total_wallets = get_wallet_count(), state;
            get_common_wallet_list(wallet_info);   // wallet list common to both cards
            for (uint8_t i = 0; i < total_wallets; i++) {
                uint8_t match = 0;
                state = get_wallet_state(i);
                if (state != VALID_WALLET) continue;
                wallet_name = get_wallet_name(i);
                wallet_id = get_wallet_id(i);
                for (int j = 0; j < MAX_WALLETS_ALLOWED; j++) {
                    if (!wallet_info[j][0] || !wallet_info[j][1]) continue;
                    if (memcmp(wallet_name, wallet_info[j][0], NAME_SIZE) == 0 && memcmp(wallet_id, wallet_info[j][1], WALLET_ID_SIZE) == 0) {
                        match = 1;
                        break;
                    }
                }
                if (!match) {
                    factory_reset_mismatch_wallet_index = i;
                    flow_level.level_three = FACTORY_RESET_CANCEL;
                    return;
                }
            }
            flow_level.level_three = FACTORY_RESET_ERASING;
        } break;

        case FACTORY_RESET_ERASING:
            if (get_display_rotation() == LEFT_HAND_VIEW)
                ui_rotate();
            sec_flash_erase();
            flash_erase();
            logger_reset_flash();
            BSP_reset();
            break;

        default:
            reset_flow_level();
    }
}

static void tap_card_backend(uint8_t *recv_apdu, uint16_t *recv_len) {
    while (1) {
        tap_card_data.acceptable_cards = acceptable_cards;
        memcpy(tap_card_data.family_id, get_family_id(), FAMILY_ID_SIZE);
        tap_card_data.tapped_card = 0;
        if (!tap_card_applet_connection())
            break;

        tap_card_data.status = nfc_list_all_wallet(recv_apdu, recv_len);
        *recv_len -= 2;
        if (tap_card_data.status == SW_NO_ERROR || tap_card_data.status == SW_RECORD_NOT_FOUND) {
            tap_card_data.retries = 5;
            acceptable_cards = tap_card_data.acceptable_cards;
            flow_level.level_three++;
            buzzer_start(BUZZER_DURATION);
            instruction_scr_change_text(ui_text_remove_card_prompt, true);
            nfc_detect_card_removal();
            lv_obj_clean(lv_scr_act());
            break;
        } else if (tap_card_handle_applet_errors()) {
            break;
        }
    }
}

static void decode_wallet_info_list(uint8_t *wallet_info[MAX_WALLETS_ALLOWED][2], uint8_t *data, uint16_t len) {
    uint16_t wallet_count = 0;
    memzero(wallet_info, MAX_WALLETS_ALLOWED * 2 * sizeof(size_t));
    for (uint16_t index = 1 + 4; index < len; wallet_count++) {
        ASSERT(data[index++] == INS_NAME);
        wallet_info[wallet_count][0] = &data[++index];
        index += data[index - 1];
        ASSERT(data[index++] == INS_WALLET_ID);
        wallet_info[wallet_count][1] = &data[++index];
        index += data[index - 1] + 4;
    }
}

/**
 * @brief Prepares a list of wallets that are common to the two tapped cards.
 */
static void get_common_wallet_list(uint8_t *wallet_info[MAX_WALLETS_ALLOWED][2]) {
    uint8_t *wallet_info_list[MAX_WALLETS_ALLOWED][2];

    decode_wallet_info_list(wallet_info, card1, card1_len);
    decode_wallet_info_list(wallet_info_list, card2, card2_len);
    for (int index = 0; index < MAX_WALLETS_ALLOWED; index++) {
        uint8_t match = 0;
        if (wallet_info[index][0] == NULL || wallet_info[index][1] == NULL) continue;
        for (uint8_t i = 0; i < MAX_WALLETS_ALLOWED; i++) {
            if (wallet_info_list[i][0] == NULL || wallet_info_list[i][1] == NULL) continue;
            if (memcmp(wallet_info_list[i][0], wallet_info[index][0], NAME_SIZE) == 0 &&
                memcmp(wallet_info_list[i][1], wallet_info[index][1], WALLET_ID_SIZE) == 0) {
                match = 1;
                break;
            }
        }
        if (!match) {
            wallet_info[index][0] = NULL;
            wallet_info[index][1] = NULL;
        }
    }
}
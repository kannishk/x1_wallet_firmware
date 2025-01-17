#include <stdio.h>
#include <string.h>
#include <stdint.h>

void print_hex_array(const char text[], const uint8_t* arr, const uint8_t length)
{
  printf("%s %d\n", text, length);

  for (uint8_t i = 0U; i < length; i++) {
    printf("%02X", arr[i]);
  }
  printf("\n");
}

void der_to_sig(const uint8_t *der, uint8_t *sig)
{
  if (!der || !sig)
    return;
  memset(sig, 0, 64);
  uint8_t len, offset = 0;
  if (der[offset++] != 0x30) return;      // Verify the starting byte is 0x30
  offset++;                               // Skip the length byte
  if (der[offset++] != 0x02) return;      // Verify the marker byte is 0x02
  len = der[offset++];                    // Get the length of the r value
  if (len == 0x21) {
    offset++;
    len--;
  }
  memcpy(sig, der + offset, len);         // Copy the r component of signature
  offset += len;
  if (der[offset++] != 0x02) return;      // Verify the marker byte is 0x02
  len = der[offset++];                    // Get the length of the s value
  if (len == 0x21) {
    offset++;
    len--;
  }
  memcpy(sig + 32, der + offset, len);
}

void test1() {
  uint8_t der[] = {0x30, 0x44,
      0x02, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
      0x02, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test1 failed\n");
  } else {
    printf("test1 passed\n");
  }
}

void test2() {
  uint8_t der[] = {0x30, 0x45,
                   0x02, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
                   0x02, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test2 failed\n");
  } else {
    printf("test2 passed\n");
  }
}

void test3() {
  uint8_t der[] = {0x30, 0x46,
                   0x02, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
                   0x02, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test3 failed\n");
  } else {
    printf("test3 passed\n");
  }
}

void test4() {
  uint8_t der[] = {0x30, 0x45,
                   0x02, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
                   0x02, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test4 failed\n");
  } else {
    printf("test4 passed\n");
  }
}

void test5() {
  uint8_t der[] = {0x40, 0x45,
                   0x02, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
                   0x02, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test5 failed\n");
  } else {
    printf("test5 passed\n");
  }
}

void test6() {
  uint8_t der[] = {0x30, 0x45,
                   0x03, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
                   0x02, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test6 failed\n");
  } else {
    printf("test6 passed\n");
  }
}

void test7() {
  uint8_t der[] = {0x30, 0x45,
                   0x02, 0x20, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16,
                   0x03, 0x21, 0x00, 0xae, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16
  };
  uint8_t expected_sig[64] = {0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16, 0x7e, 0xf1, 0x4e, 0x84, 0xe9, 0xe5, 0xfb, 0x0a, 0x97, 0x26, 0x28, 0xcf, 0x45, 0x68, 0xc4, 0x03, 0x16},
          actual_sig[64] = {0};

  der_to_sig(der, actual_sig);
//  print_hex_array("expected: ", expected_sig, sizeof(expected_sig));
//  print_hex_array("actual: ", actual_sig, sizeof(actual_sig));
  if (memcmp(expected_sig, actual_sig, 64) != 0) {
    printf("test7 failed\n");
  } else {
    printf("test7 passed\n");
  }
}

int main(void) {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
}

#define _GNU_SOURCE
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int IsPassOk(void);

int main(void) {
  int PwStatus;
  puts("Enter password:");
  PwStatus = IsPassOk();
  if (PwStatus == 0) {
    printf("Bad password!\n");
    exit(1);
  } else {
    printf("Access granted!\n");  // Строка для которой нужно выяснить адрес
  }
  return 0;
}

int IsPassOk(void) {
  char Pass[12];
  gets(Pass);
  return 0 == strcmp(Pass, "test");
}

//"\xdf\x11\x40\x00\x00\x00\x00\x00"
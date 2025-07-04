#include <stdio.h>
#include <string.h>

#define MAX_LEN 256

char* find_substring(const char* str, const char* substr) {
  if (*substr == '\0') return (char*)str;
  for (; *str; str++) {
    const char *s = str, *p = substr;
    while (*s != '\0' && *p != '\0' && *s == *p) {
      s++;
      p++;
    }
    if (*p == '\0') return (char*)str;
  }
  return NULL;
}

int main() {
  char str[MAX_LEN];
  char substr[MAX_LEN];

  printf("input string: ");
  fgets(str, MAX_LEN, stdin);

  printf("input substring: ");
  fgets(substr, MAX_LEN, stdin);

  substr[strcspn(substr, "\n")] = '\0';
  str[strcspn(str, "\n")] = '\0';

  char* result = find_substring(str, substr);

  if (result) {
    printf("found: %s\n", result);
  } else {
    printf("not found\n");
  }

  return 0;
}
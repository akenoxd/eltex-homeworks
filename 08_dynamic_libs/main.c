#include <stdio.h>

#include "calc.h"

void print_menu();
void op_add();
void op_sub();
void op_mul();
void op_div();

int main() {
  while (1) {
    printf("\n");
    print_menu();
    int choice = 0;
    scanf("%d", &choice);
    while (getchar() != '\n');
    switch (choice) {
      case 1:
        op_add();
        break;
      case 2:
        op_sub();
        break;
      case 3:
        op_mul();
        break;
      case 4:
        op_div();
        break;
      case 5:
        return 0;
      default:
        printf("Wrong input!\n");
    }
  }
}

void print_menu() {
  printf("1. add\n");
  printf("2. subtract\n");
  printf("3. multiply\n");
  printf("4. divide\n");
  printf("5. exit\n");
  printf("choose [1-5]: ");
}

void op_add() {
  int a, b;
  printf("Enter two numbers to add: ");
  scanf("%d %d", &a, &b);
  printf("Result: %d\n", a + b);
}

void op_sub() {
  int a, b;
  printf("Enter two numbers to subtract: ");
  scanf("%d %d", &a, &b);
  printf("Result: %d\n", sub(a, b));
}

void op_mul() {
  int a, b;
  printf("Enter two numbers to multiply: ");
  scanf("%d %d", &a, &b);
  printf("Result: %d\n", mul(a, b));
}

void op_div() {
  int a, b;
  printf("Enter two numbers to divide: ");
  scanf("%d %d", &a, &b);
  if (b == 0) {
    printf("Error: Division by zero!\n");
  } else {
    printf("Result: %d\n", divide(a, b));
  }
}
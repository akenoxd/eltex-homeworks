#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

void print_menu() {
  printf("1. Add\n");
  printf("2. Delete\n");
  printf("3. Search by name\n");
  printf("4. Show all\n");
  printf("5. Exit\n\n");
  printf("Input number [1-5]: ");
}

void add(Node **head) {
  abonent ab;
  printf("Enter name: ");
  scanf("%9s", ab.name);
  printf("Enter second name: ");
  scanf("%9s", ab.second_name);
  printf("Enter phone number: ");
  scanf("%9s", ab.tel);
  insert(head, &ab);
  printf("Added!\n");
}

void delete(Node **head) {
  char name[10];
  printf("Enter name to delete: ");
  scanf("%9s", name);
  if (delete_node(head, name))
    printf("Not found\n");
  else
    printf("Deleted!\n");
}

void search(Node *head) {
  char name[10];
  printf("Enter name to search: ");
  scanf("%9s", name);
  Node *found = find_by_name(head, name);
  if (found) {
    printf("Found: %s %s %s\n", found->data.name, found->data.second_name,
           found->data.tel);
  } else {
    printf("Not found.\n");
  }
}

int main() {
  Node *head = NULL;
  while (1) {
    printf("\n");
    print_menu();
    int choice = 0;
    scanf("%d", &choice);
    // while (getchar() != '\n');
    switch (choice) {
      case 1:
        add(&head);
        break;
      case 2:
        delete (&head);
        break;
      case 3:
        search(head);
        break;
      case 4:
        print_list(head);
        break;
      case 5:
        free_list(head);
        return 0;
      default:
        printf("Wrong input!\n");
    }
  }
}
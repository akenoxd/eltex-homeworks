#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *create_node(const abonent *data) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (!node) return NULL;
  node->data = *data;
  node->prev = NULL;
  node->next = NULL;
  return node;
}

void insert(Node **head, const abonent *data) {
  Node *new_node = create_node(data);
  if (!new_node) return;

  // Добавление в пустой список
  if (*head == NULL) {
    *head = new_node;
    return;
  }
  Node *curr = *head;
  while (curr && strcmp(data->name, curr->data.name) > 0) {
    if (!curr->next) break;
    curr = curr->next;
  }

  // Добавление в начало списка
  if (curr == *head && strcmp(data->name, curr->data.name) < 0) {
    new_node->next = *head;
    (*head)->prev = new_node;
    *head = new_node;
  }
  // Добавление в конец списка
  else if (!curr->next && strcmp(data->name, curr->data.name) > 0) {
    curr->next = new_node;
    new_node->prev = curr;
  }
  // Добавление в середину списка
  else {
    new_node->prev = curr->prev;
    new_node->next = curr;
    if (curr->prev) curr->prev->next = new_node;
    curr->prev = new_node;
    if (curr == *head) *head = new_node;
  }
}

int delete_node(Node **head, char *name) {
  Node *curr = *head;
  while (curr) {
    if (strcmp(curr->data.name, name) == 0) {
      if (curr->prev)
        curr->prev->next = curr->next;
      else
        *head = curr->next;
      if (curr->next) curr->next->prev = curr->prev;
      free(curr);
      return 0;
    }
    curr = curr->next;
  }
  return 1;
}

Node *find_by_name(Node *head, const char *name) {
  Node *curr = head;
  while (curr) {
    if (strcmp(curr->data.name, name) == 0) return curr;
    curr = curr->next;
  }
  return NULL;
}

void free_list(Node *head) {
  while (head) {
    Node *tmp = head;
    head = head->next;
    free(tmp);
  }
}

void print_list(const Node *head) {
  printf("      Name|Second name|Phone number\n");
  while (head) {
    printf("%10s|%11s|%12s\n", head->data.name, head->data.second_name,
           head->data.tel);
    head = head->next;
  }
}
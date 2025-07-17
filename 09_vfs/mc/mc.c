#include "mc.h"

#include <stdlib.h>
#include <string.h>

void free_entries(Panel* panel) {
  if (panel->entries) {
    for (int i = 0; i < panel->entry_count; ++i) free(panel->entries[i]);
    free(panel->entries);
    panel->entries = NULL;
  }
  panel->entry_count = 0;
}

void panel_init(Panel* panel) {
  snprintf(panel->path, sizeof(panel->path), "/home");

  panel->entries = NULL;
  panel->entry_count = 0;
  panel->selected = 0;
  panel->scroll = 0;
  panel->active = 0;
}

void load_dir(Panel* panel) {
  free_entries(panel);

  DIR* dir = opendir(panel->path);
  if (!dir) {
    panel->entries = NULL;
    panel->entry_count = 0;
    return;
  }

  struct dirent* entry;
  int capacity = 32;
  panel->entries = malloc(capacity * sizeof(char*));
  panel->entry_count = 0;

  while ((entry = readdir(dir)) != NULL) {
    if (panel->entry_count >= capacity) {
      capacity *= 2;
      panel->entries = realloc(panel->entries, capacity * sizeof(char*));
    }
    size_t len = strlen(entry->d_name) + 1;
    panel->entries[panel->entry_count] = malloc(len);
    if (panel->entries[panel->entry_count])
      memcpy(panel->entries[panel->entry_count], entry->d_name, len);
    panel->entry_count++;
  }
  closedir(dir);

  // Сортировка (опционально)
  // qsort(panel->entries, panel->entry_count, sizeof(char*), cmp);

  panel->selected = 0;
  panel->scroll = 0;
}

void draw_panel(Panel* panel, WINDOW* win, int w, int h) {
  werase(win);
  wclear(win);
  box(win, 0, 0);

  int max_show = h - 2;
  int start = panel->scroll;
  int end = start + max_show;
  if (end > panel->entry_count) end = panel->entry_count;

  for (int i = start; i < end; ++i) {
    int row = 1 + (i - start);
    if (panel->active && i == panel->selected) {
      wattron(win, COLOR_PAIR(2));
      mvwprintw(win, row, 1, "%-*s", w - 2, panel->entries[i]);
      wattroff(win, COLOR_PAIR(2));
    } else {
      mvwprintw(win, row, 1, "%-*s", w - 2, panel->entries[i]);
    }
  }
  // Можно добавить отображение пути панели:
  mvwprintw(win, 0, 2, "[%s]", panel->path);
}

void draw_decoration() {
  box(stdscr, 0, 0);
  attron(COLOR_PAIR(1));
  mvprintw(LINES - 1, 0, "Press 'q' to quit, 'Tab' to switch panels");
  attroff(COLOR_PAIR(1));
}

void panel_change_active(Panel* panel) {
  panel[0].active ^= 1;
  panel[1].active ^= 1;
}
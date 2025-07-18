#include "mc.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void free_entries(Panel *panel) {
  if (panel->entries) {
    for (int i = 0; i < panel->entry_count; ++i)
      free(panel->entries[i]);
    free(panel->entries);
    panel->entries = NULL;
  }
  panel->entry_count = 0;
}

void panel_init(Panel *panel) {
  char *cwd = getenv("PWD");
  if (cwd == NULL)
    snprintf(panel->path, sizeof(panel->path), "/");
  else
    snprintf(panel->path, sizeof(panel->path), "%s", cwd);

  panel->entries = NULL;
  panel->entry_count = 0;
  panel->selected = 0;
  panel->scroll = 0;
  panel->active = 0;
}

int cmp(const void *a, const void *b) { // qsort
  const char *ia = *(const char **)a;
  const char *ib = *(const char **)b;
  return strcmp(ia, ib);
}

void load_dir(Panel *panel) {
  free_entries(panel);

  DIR *dir = opendir(panel->path);
  if (!dir) {
    panel->entries = NULL;
    panel->entry_count = 0;
    return;
  }

  struct dirent *entry;
  int capacity = 32;
  panel->entries = malloc(capacity * sizeof(char *));
  panel->entry_count = 0;

  panel->entries[panel->entry_count] = malloc(3);
  sprintf(panel->entries[panel->entry_count], "..");
  panel->entry_count++;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    if (panel->entry_count >= capacity) {
      capacity *= 2;
      panel->entries = realloc(panel->entries, capacity * sizeof(char *));
    }

    size_t len = strlen(entry->d_name) + 1;
    panel->entries[panel->entry_count] = malloc(len);

    char full_path[PATH_MAX + 256];
    snprintf(full_path, sizeof(full_path), "%s/%s", panel->path,
             entry->d_name); // полный путь чтобы проверить с помощью stat
                             // директория или нет

    struct stat sb;
    if (stat(full_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
      len += 1;
      sprintf(panel->entries[panel->entry_count], "/%s", entry->d_name);
    } else {
      sprintf(panel->entries[panel->entry_count], "%s", entry->d_name);
    }
    panel->entry_count++;
  }
  closedir(dir);

  qsort(panel->entries + 1, panel->entry_count - 1, sizeof(char *), cmp);
}

void draw_panel(Panel *panel, WINDOW *win, int w, int h) {
  werase(win);
  wclear(win);
  box(win, 0, 0);

  int max_show = h - 2;

  if (panel->selected < panel->scroll)
    panel->scroll = panel->selected;
  else if (panel->selected >= panel->scroll + max_show)
    panel->scroll = panel->selected - max_show + 1;

  int start = panel->scroll;
  int end = start + max_show;
  if (end > panel->entry_count)
    end = panel->entry_count;

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

  int path_len = strlen(panel->path);

  // стираем часть пути с начала чтобы влезло в окно
  if (path_len > w - 4) {
    int trunc_len = w - 4 - 1;
    mvwprintw(win, 0, 2, "[~%.*s]", trunc_len,
              panel->path + path_len - trunc_len);
  } else {
    mvwprintw(win, 0, 2, "[%s]", panel->path);
  }
}

void panel_change_active(Panel *panel) {
  panel[0].active ^= 1;
  panel[1].active ^= 1;
}

void change_dir(Panel *panel) {
  if (panel->selected == 0) { // ".."
    char prev_dir[PATH_MAX];
    strcpy(prev_dir, strrchr(panel->path, '/'));
    strrchr(panel->path, '/')[0] = '\0';
    if (strlen(panel->path) == 0) {
      snprintf(panel->path, sizeof(panel->path), "/");
    }
    load_dir(panel);
    for (int i = 0; i < panel->entry_count; i++) {
      if (strcmp(panel->entries[i], prev_dir) == 0) {
        panel->selected = i;
        break;
      }
    }
    return;
  }

  if (panel->entries[panel->selected][0] == '/') {
    char new_path[PATH_MAX];
    if (strcmp(panel->path, "/") == 0 && strlen(panel->path) == 1) {
      sprintf(new_path, "%s", panel->entries[panel->selected]);
    } else {
      sprintf(new_path, "%s%s", panel->path, panel->entries[panel->selected]);
    }
    strcpy(panel->path, new_path);
    load_dir(panel);
    panel->selected = 0;
  }
  return;
}
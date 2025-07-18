#ifndef MC_H
#define MC_H
#include <dirent.h>
#include <linux/limits.h>
#include <ncurses.h>

typedef struct {
  char path[PATH_MAX];
  char **entries;
  int entry_count;
  int selected;
  int scroll;
  int active;
} Panel;

void panel_change_active(Panel *panel);
void panel_init(Panel *panel);
void load_dir(Panel *panel);
void draw_panel(Panel *panel, WINDOW *win, int w, int h);
void change_dir(Panel *panel);
#endif // MC_H
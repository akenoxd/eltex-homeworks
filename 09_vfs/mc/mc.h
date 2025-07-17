#include <dirent.h>
#include <linux/limits.h>
#include <ncurses.h>

typedef struct {
  char path[PATH_MAX];  // Текущий путь
  char** entries;       // Массив имён файлов/папок
  int entry_count;      // Количество файлов/папок
  int selected;         // Индекс выбранного файла
  int scroll;  // Для прокрутки, если файлов больше чем строк
  int active;
} Panel;

void panel_change_active(Panel* panel);
void panel_init(Panel* panel);
void load_dir(Panel* panel);
void draw_panel(Panel* panel, WINDOW* win, int w, int h);
void draw_decoration();
#include <ncurses.h>
#include <string.h>

#include "mc.h"

int main() {
  Panel panels[2];

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_WHITE, COLOR_CYAN);
  bkgd(COLOR_PAIR(1));
  int panel_width = COLS / 2;
  int panel_height = LINES - 2;
  mvprintw(0, 0, "Press 'qq' to exit");

  WINDOW *win_panel1 = newwin(panel_height, panel_width, 1, 0);
  WINDOW *win_panel2 = newwin(panel_height, panel_width, 1, panel_width);
  wbkgd(win_panel1, COLOR_PAIR(1));
  wbkgd(win_panel2, COLOR_PAIR(1));

  panel_init(&panels[0]);
  panel_init(&panels[1]);
  panels[0].active = 1;
  int active_panel = 0;

  load_dir(&panels[0]);
  load_dir(&panels[1]);

  wrefresh(win_panel1);
  wrefresh(win_panel2);
  refresh();

  while (1) {
    draw_panel(&panels[0], win_panel1, panel_width, panel_height);
    draw_panel(&panels[1], win_panel2, panel_width, panel_height);

    wrefresh(win_panel1);
    wrefresh(win_panel2);

    switch (getch()) {
    case KEY_UP:
      if (panels[active_panel].selected > 0) {
        panels[active_panel].selected--;
      }
      break;
    case KEY_DOWN:
      if (panels[active_panel].selected <
          panels[active_panel].entry_count - 1) {
        panels[active_panel].selected++;
      }
      break;
    case KEY_ENTER:
    case 10: // Enter key
      change_dir(&panels[active_panel]);
      break;
    case '\t':
      panel_change_active(panels);
      active_panel ^= 1;
      break;
    default:
      break;
    case 'q':
      if (getch() == 'q') {
        delwin(win_panel1);
        delwin(win_panel2);
        endwin();
        return 0;
      }
      break;
    }
  }

  delwin(win_panel1);
  delwin(win_panel2);
  endwin();
  return 0;
}
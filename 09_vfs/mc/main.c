#include <dirent.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>

#include "mc.h"

volatile sig_atomic_t need_resize = 0;

// void handle_winch(int sig) { need_resize = sig; }

int main() {
  Panel panels[2];

  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  init_pair(2, COLOR_WHITE, COLOR_CYAN);  // selected file color

  // bkgd(COLOR_PAIR(1));
  // mvprintw(0, 0, "mc hello");

  // signal(SIGWINCH, handle_winch);

  int panel_width = COLS / 2;
  int panel_height = LINES - 2;
  int active_panel = 0;

  WINDOW *win_panel1 = newwin(panel_height, panel_width, 1, 0);
  WINDOW *win_panel2 = newwin(panel_height, panel_width, 1, panel_width);

  panel_init(&panels[0]);
  panel_init(&panels[1]);
  panels[0].active = 1;

  load_dir(&panels[0]);
  load_dir(&panels[1]);

  wrefresh(win_panel1);
  wrefresh(win_panel2);
  refresh();

  // need_resize = 1;

  while (1) {
    if (need_resize) {
      need_resize = 0;
      endwin();
      refresh();
      clear();

      delwin(win_panel1);
      delwin(win_panel2);

      win_panel1 = newwin(panel_height, panel_width, 1, 0);
      win_panel2 = newwin(panel_height, panel_width, 1, panel_width);
    }

    draw_panel(&panels[0], win_panel1, COLS / 2, LINES - 2);
    draw_panel(&panels[1], win_panel2, COLS / 2, LINES - 2);
    // draw_decoration();

    mvwprintw(win_panel1, 30, 2, "[%d]", LINES);

    wrefresh(win_panel1);
    wrefresh(win_panel2);
    // refresh();
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
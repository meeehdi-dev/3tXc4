#include <ncurses.h>
#include "ui/ui.hpp"
#include "misc/misc.hpp"

int main ( int argc , char **argv ) {

    initscr ();
    timeout ( -1 );
    raw ();
    keypad ( stdscr , TRUE );
    noecho ();
    curs_set ( 0 );

    /*    while ( true ) { // KEY TESTING

          int row,col;

          getmaxyx(stdscr,row,col);

          int Key = getch ();

          clear ();

          if ( Key == -1 )
          mvprintw(row/2,col/2 - 15,"OMG WRITE SOMETHING!");
          else
          mvprintw(row/2,col/2,"%d",Key);

          refresh ();
          }
     */
    ui ();

    return 0;
}

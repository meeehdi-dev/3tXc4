#include <ncurses.h>
#include <fstream>
#include <ctime>
#include <list>
#include <algorithm>
#include <iterator>
#include "grid.hpp"
#include "../misc/misc.hpp"

bool grid::initXO = false;
std::string grid::ID = "";

grid::grid ( void ) { // grid c-tor #1

	int i , j;

	height = 5;
	width = 5;

	XO = new int*[height];
	for ( i = 0 ; i < height ; ++i )
		XO[i] = new int[width];

	for ( i = 0 ; i < height ; ++i ) {

		for ( j = 0 ; j < width ; ++j )
			XO[i][j] = 0;
	}

	alignWinSize = 5;
	alignWinTotal = 1;

	initXO = false;
}

grid::grid ( int newHeight , int newWidth , int newAlignWinSize , int newAligneWinTotal ) { // grid c-tor #2

	int i , j;

	height = newHeight;
	width = newWidth;

	XO = new int*[height];
	for ( i = 0 ; i < height ; ++i )
		XO[i] = new int[width];

	for ( i = 0 ; i < height ; ++i ) {

		for ( j = 0 ; j < width ; ++j )
			XO[i][j] = 0;
	}

	alignWinSize = newAlignWinSize;
	alignWinTotal = newAligneWinTotal;

	initXO = true;

	time_t now = time ( 0 );
	tm *ltm = localtime ( &now );
	int year = ltm->tm_year + 1900;
	int month = ltm->tm_mon + 1;
	int day = ltm->tm_mday;
	int hour = ltm->tm_hour;
	int min = ltm->tm_min;
	int sec = ltm->tm_sec;

	std::string str_year ( std::to_string ( year ) );

	std::string str_month ( "" );
	if ( month < 10 ) {
		str_month += "0";
	}
	str_month += std::to_string ( month );

	std::string str_day ( "" );
	if ( day < 10 ) {
		str_day += "0";
	}
	str_day += std::to_string ( day );

	std::string str_hour ( "" );
	if ( hour < 10 ) {
		str_hour += "0";
	}
	str_hour += std::to_string ( hour );

	std::string str_min ( "" );
	if ( min < 10 ) {
		str_min += "0";
	}
	str_min += std::to_string ( min );

	std::string str_sec ( "" );
	if ( sec < 10 ) {
		str_sec += "0";
	}
	str_sec += std::to_string ( sec );

	std::string date = str_year + str_month + str_day + str_hour + str_min + str_sec;

	ID = date;
}

grid::grid ( std::string gameID ) { // grid c-tor #3

	std::ifstream input ( STR_SAVE_REP + "/" + STR_SAVE_PRE + gameID + "." + STR_SAVE_EXT );
	if ( !input ) {

		int row , col;

		getmaxyx ( stdscr , row , col );

		clear ();
		mvprintw ( row / 2 , ( col - STR_FAIL_LOAD.length() + 1 ) / 2 , STR_FAIL_LOAD.c_str() );
		refresh ();
		wait ( 2000 );

		initXO = false;
	}
	else {

		int i , j;
		int buff;

		ID = gameID;

		input >> height >> width;
		input >> alignWinSize >> alignWinTotal;

		XO = new int*[height];
		for ( i = 0 ; i < height ; ++i )
			XO[i] = new int[width];


		for ( i = 0 ; i < height ; ++i ) {

			for ( j = 0 ; j < width ; ++j ) {

				input >> buff;

				switch ( buff ) {

					case 1:
						XO[i][j] = 1;
						break;

					case 2:
						XO[i][j] = 2;
						break;

					default:
						XO[i][j] = 0;
						break;
				}
			}
		}

		input.close ();

		initXO = true;
	}
}

grid::~grid ( void ) { // grid d-tor

	int i;

	for ( i = 0 ; i < height ; ++i )
		delete []XO[i];
	delete []XO;

	initXO = false;
}

void grid::save ( void ) { // grid save

	std::ofstream check ( STR_SAVE_REP + "/" + STR_SAVE_CHK + "." + STR_SAVE_EXT );

	if ( !check ) {

		std::string command = "mkdir -p " + STR_SAVE_REP;
		system(command.c_str());
	}

	if ( check )
		check.close ();

	std::ifstream indexread ( STR_SAVE_REP + "/" + STR_SAVE_IND + "." + STR_SAVE_EXT );

	if ( indexread ) {

		std::list<std::string> listID;
		std::list<std::string>::iterator it;
		std::string buff;

		while ( !indexread.eof() ) {

			indexread >> buff;
			if ( std::find ( listID.begin () , listID.end () , buff ) == listID.end () )
				listID.push_back(buff);
		}

		if ( std::find ( listID.begin () , listID.end () , ID ) == listID.end () )
			listID.push_back(ID);

		indexread.close ();

		std::ofstream indexwrite ( STR_SAVE_REP + "/" + STR_SAVE_IND + "." + STR_SAVE_EXT );

		for ( it = listID.begin() ; it != listID.end () ; ++it )
			indexwrite << (*it) << std::endl;

		indexwrite.close();
	}
	else {

		std::ofstream index ( STR_SAVE_REP + "/" + STR_SAVE_IND + "." + STR_SAVE_EXT );

		index << ID << std::endl;

		index.close ();
	}

	std::ofstream save ( STR_SAVE_REP + "/" + STR_SAVE_PRE + ID + "." + STR_SAVE_EXT );

	save << height << " " << width << std::endl;
	save << alignWinSize << " " << alignWinTotal << std::endl;

	int i , j;

	for ( i = 0 ; i < height ; ++i ) {

		for ( j = 0 ; j < width ; ++j ) {

			save << XO[i][j] << " ";
		}

		save << std::endl;
	}

	save.close ();

	return;
}

void grid::insert ( int player , int pos ) { // grid insert X or O

	int i;
	int row , col;

	while ( true ) {

		getmaxyx ( stdscr , row , col );

		if ( pos < 0 )
			pos = 0;

		if ( pos > width - 1 )
			pos = width - 1;

		clear ();

		draw ();

		mvprintw ( ( row / 2 ) - ( height*2 / 2 ) - 1 , ( col / 2 ) - ( width*4 / 2 ) , "  " );

		for ( i = 0 ; i < pos ; ++i )
			printw ( "    " );

		switch ( player ) {

			case 1:
				printw ( "X" );
				break;

			case 2:
				printw ( "O" );
				break;

			default:
				break;
		}

		refresh ();

		int Key = getch ();

		switch ( Key ) {

			case KEY_LEFT:
				pos--;
				break;

			case KEY_RIGHT:
				pos++;
				break;

			default:
				break;
		}

		if ( Key == ENTER ) {

			if ( XO[0][pos] == 0 ) {

				XO[0][pos] = player;
				break;
			}
		}
	}
}

int grid::checkWin ( void ) { // grid win check

	int h , i , j , k , l;
	int countX , countO;

	int alignX ( 0 );
	int alignO ( 0 );

	for ( i = 0 ; i < height ; ++i ) {

		for ( j = 0 ; j <= ( width - alignWinSize ) ; ++j ) {

			countX = 0;
			countO = 0;

			for ( k = j ; k < j + alignWinSize ; ++k ) {

				if ( XO[i][k] == 1 )
					countX++;

				if ( XO[i][k] == 2 )
					countO++;
			}

			if ( countX == alignWinSize ) {

				alignX++;
				countX = 0;
			}

			if ( countO == alignWinSize ) {

				alignO++;
				countO = 0;
			}
		}
	}

	for ( i = 0 ; i <= ( height - alignWinSize ) ; ++i ) {

		for ( j = 0 ; j < width ; ++j ) {

			countX = 0;
			countO = 0;

			for ( k = i ; k < i + alignWinSize ; ++k ) {

				if ( XO[k][j] == 1 )
					countX++;

				if ( XO[k][j] == 2 )
					countO++;
			}

			if ( countX == alignWinSize ) {

				alignX++;
				countX = 0;
			}

			if ( countO == alignWinSize ) {

				alignO++;
				countO = 0;
			}
		}
	}

	for ( h = height - alignWinSize ; h >= 0 ; --h ) {

		i = h;
		j = 0;

		while ( ( i <= ( height - alignWinSize ) ) && ( j <= (width - alignWinSize ) ) ) {

			countX = 0;
			countO = 0;

			for ( k = i , l = j ; k < i + alignWinSize , l < j + alignWinSize ; ++k , ++l ) {

				if ( XO[k][l] == 1 )
					countX++;

				if ( XO[k][l] == 2 )
					countO++;
			}

			if ( countX == alignWinSize ) {

				alignX++;
				countX = 0;
			}

			if ( countO == alignWinSize ) {

				alignO++;
				countO = 0;
			}

			++i;
			++j;
		}
	}

	for ( h = 1 ; h <= width - alignWinSize ; ++h ) {

		i = 0;
		j = h;

		while ( ( i <= ( height - alignWinSize ) ) && ( j <= ( width - alignWinSize ) ) ) {

			countX = 0;
			countO = 0;

			for ( k = i , l = j ; k < i + alignWinSize , l < j + alignWinSize ; ++k , ++l ) {

				if ( XO[k][l] == 1 )
					countX++;

				if ( XO[k][l] == 2 )
					countO++;
			}

			if ( countX == alignWinSize ) {

				alignX++;
				countX = 0;
			}

			if ( countO == alignWinSize ) {

				alignO++;
				countO = 0;
			}

			++i;
			++j;
		}
	}

	for ( h = width - alignWinSize ; h >= 0 ; --h ) {

		i = height - 1;
		j = h;

		while ( ( i >= ( alignWinSize - 1 ) ) && ( j <= ( width - alignWinSize ) ) ) {

			countX = 0;
			countO = 0;

			for ( k = i , l = j ; k >= 0 , l < j + alignWinSize ; --k , ++l ) {

				if ( XO[k][l] == 1 )
					countX++;

				if ( XO[k][l] == 2 )
					countO++;
			}

			if ( countX == alignWinSize ) {

				alignX++;
				countX = 0;
			}

			if ( countO == alignWinSize ) {

				alignO++;
				countO = 0;
			}

			--i;
			++j;
		}
	}

	for ( h = height - 2 ; h >= alignWinSize - 1 ; --h ) {

		i = h;
		j = 0;

		while ( ( i >= ( alignWinSize - 1 ) ) && ( j <= ( width - alignWinSize ) ) ) {

			countX = 0;
			countO = 0;

			for ( k = i , l = j ; k >= 0 , l < j + alignWinSize ; --k , ++l ) {

				if ( XO[k][l] == 1 )
					countX++;

				if ( XO[k][l] == 2 )
					countO++;
			}

			if ( countX == alignWinSize ) {

				alignX++;
				countX = 0;
			}

			if ( countO == alignWinSize ) {

				alignO++;
				countO = 0;
			}

			--i;
			++j;
		}
	}

	if ( alignX >= alignWinTotal )
		alignX = 1;
	else
		alignX = 0;

	if ( alignO >= alignWinTotal )
		alignO = 2;
	else
		alignO = 0;

	return alignX+alignO;
}

void grid::rotate ( bool clockwise ) { // grid rotation

	clear ();

	int i , j;
	int next_i , next_j;

	int **TMP_XO;

	TMP_XO = new int*[width];
	for ( i = 0 ; i < width ; ++i )
		TMP_XO[i] = new int[height];

	for ( i = 0 ; i < height ; ++i ) {

		for ( j = 0 ; j < width ; ++j ) {

			if ( clockwise ) {

				next_i = j;
				next_j = height - i - 1;
			}
			else {

				next_i = width - j - 1;
				next_j = i;
			}

			TMP_XO[next_i][next_j] = XO[i][j];
		}
	}

	int tmp = width;
	width = height;
	height = tmp;

	XO = new int*[height];
	for ( i = 0 ; i < height ; ++i )
		XO[i] = new int[width];

	for ( i = 0 ; i < height ; ++i ) {

		for ( j = 0 ; j < width ; ++j )
			XO[i][j]=TMP_XO[i][j];
	}

	draw ();

	for ( i = 0 ; i < height ; ++i )
		delete []TMP_XO[i];
	delete []TMP_XO;
}

void grid::gravitate ( void ) { // grid gravitation

	int i , j;

	bool modified ( true );

	while ( modified ){

		modified = false;

		for ( i = 0 ; i < height-1 ; ++i ) {

			clear ();

			draw ();

			wait ( 200 );

			for ( j = 0 ; j < width ; ++j ) {

				if ( XO[i+1][j] != 1 && XO[i+1][j] != 2 && XO[i][j] != 0 ) {

					XO[i+1][j] = XO[i][j];
					XO[i][j] = 0;

					modified = true;
				}
			}
		}
	}
}

void grid::play ( int player ) { // grid play (insert or rotate)

	int selected ( 1 );
	std::string STR_SELECTED;
	int TAB;
	int row , col;
	int Key;

	while ( true ) {

		getmaxyx ( stdscr , row , col );

		if ( selected < 1 )
			selected = 1;

		if ( selected > 2 )
			selected = 2;

		if ( selected == 1 ) {
			STR_SELECTED = STR_INSERT;
			TAB = 1;
		}

		if ( selected == 2 ) {
			STR_SELECTED = STR_ROTATE;
			TAB = 2;
		}

		clear ();

		draw ();

		attron ( A_BOLD );

		mvprintw ( 3 , ( col / 2 ) - ( STR_P1.length () / 2 ) + 1 , STR_P1.c_str () );

		mvprintw ( row - 4 , ( TAB * col / 3 ) - STR_SELECTED.length () - 2 , STR_ARROW_RIGHT.c_str () );
		mvprintw ( row - 4 , ( TAB * col / 3 ) + STR_SELECTED.length () , STR_ARROW_LEFT.c_str () );

		attroff ( A_BOLD );

		mvprintw ( row - 4 , ( col / 3 ) - ( STR_INSERT.length () / 2 ) , STR_INSERT.c_str () );
		mvprintw ( row - 4 , ( 2 * col / 3 ) - ( STR_ROTATE.length () / 2 ) , STR_ROTATE.c_str () );

		refresh ();

		Key = getch ();

		if ( Key == KEY_RIGHT )
			selected ++;

		if ( Key == KEY_LEFT )
			selected --;

		if ( Key == ENTER ) {

			if ( selected == 1 )
				insert ( player , 0 );

			if ( selected == 2 ) {

				selected = 3;

				while ( true ) {

					clear ();

					draw ();

					if ( selected < 3 )
						selected = 3;

					if ( selected > 4 )
						selected = 4;

					if ( selected == 3 ) {
						STR_SELECTED = STR_CLOCKWISE;
						TAB = 1;
					}

					if ( selected == 4 ) {
						STR_SELECTED = STR_CCLOCKWISE;
						TAB = 2;
					}

					attron ( A_BOLD );

					mvprintw ( row - 4 , ( TAB * col / 3 ) - STR_SELECTED.length () - 2 , STR_ARROW_RIGHT.c_str () );
					mvprintw ( row - 4 , ( TAB * col / 3 ) + STR_SELECTED.length () , STR_ARROW_LEFT.c_str () );

					attroff ( A_BOLD );

					mvprintw ( row - 4 , ( col / 3 ) - ( STR_CLOCKWISE.length () / 2 ) , STR_CLOCKWISE.c_str () );
					mvprintw ( row - 4 , ( 2 * col / 3 ) - ( STR_CCLOCKWISE.length () / 2 ) , STR_CCLOCKWISE.c_str () );

					refresh ();

					Key = getch  ();

					if ( Key == KEY_RIGHT )
						selected ++;

					if ( Key == KEY_LEFT )
						selected --;

					if ( Key == ENTER ) {

						if ( selected == 3 )
							rotate ( true );

						if ( selected == 4 )
							rotate ( false );

						break;
					}
				}
			}
			break;
		}

		if ( Key == ESC )
			break;
	}
}

void grid::draw ( void ) { // grid draw

	int i , j;
	int row , col;

	getmaxyx ( stdscr , row , col );

	mvprintw ( ( row / 2 ) - ( height*2 / 2 ) , ( col / 2 ) - ( width*4 / 2 ) , "|--" );
	for ( i = 0 ; i < width-1 ; ++i ) {
		printw ( "----" );
	}
	printw ( "-|" );

	for( i = 0 ; i < height ; ++i ) {

		mvprintw ( ( row / 2 ) - ( height*2 / 2 ) + ( i * 2 ) + 1 , ( col / 2 ) - ( width*4 / 2 ) , "|" );

		for ( j = 0 ; j < width ; ++j ) {

			printw ( " " );
			if ( XO[i][j]==1 ) printw ( "X" );
			else if ( XO[i][j]==2 ) printw ( "O" );
			else printw ( " " );

			printw ( " |" );
		}

		mvprintw ( ( row / 2 ) - ( height*2 / 2 ) + ( i * 2 ) + 2 , ( col / 2 ) - ( width*4 / 2 ) , "|--" );
		for ( j = 0 ; j < width-1 ; ++j ) {
			printw ( "----" );
		}
		printw ( "-|" );
	}

	refresh ();
}

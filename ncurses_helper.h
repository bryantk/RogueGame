/*

// To Compile: gcc -c -g -o helper.o helper.c 
// To Make Executable: gcc -static -g -o farmer main.o helper.o -lcurses -lcurses -ltinfo -static

*/
#pragma once

#include <curses.h>
#include <stdlib.h>
#include <string>


WINDOW * mainwin;

int getmax(int& x, int& y) {		//initializes a window from the terminal, grabs its X Y values, and terminates
	initscr();	getmaxyx(stdscr, y, x);	endwin();	system("clear");	}		// used to resize window back to pre-game

int initialize() {
    /*  Initialize ncurses  */
	system("resize -s 35 60");	system("clear");

    if ( (mainwin = initscr()) == NULL ) {	fprintf(stderr, "Error initialising ncurses.\n");	return 1; }	//if fails for some reason
    noecho();                  /*  Turn off key echoing                 */
    keypad(mainwin, TRUE);     /*  Enable the keypad for non-char keys  */

    start_color();                    /*  Initialize colours  */
	if ( has_colors() && COLOR_PAIRS >= 13 ) {
	/*  Initialize a bunch of colour pairs, where:
	        init_pair(pair number, foreground, background);
	    specifies the pair.                                  */
	init_pair(1,  COLOR_MAGENTA, COLOR_BLACK);		//	fog					RED GREEN YELLOW BLUE MAGENTA CYAN WHITE BLACK
	init_pair(2,  COLOR_RED,   	 COLOR_BLACK);		//	walls				            x           x            x          
	init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);		//	doors		yellow
	init_pair(4,  COLOR_GREEN,   COLOR_BLACK);		//  fog walls
	init_pair(5,  COLOR_BLUE, 	 COLOR_BLACK);		//	
	init_pair(6,  COLOR_CYAN,    COLOR_BLACK);		//	
	init_pair(7,  COLOR_WHITE,   COLOR_BLACK);		//	
	init_pair(8,  COLOR_BLACK,   COLOR_BLACK);		//
	init_pair(9,  COLOR_BLACK,   COLOR_WHITE);		//walls
	init_pair(10, COLOR_BLACK,   COLOR_MAGENTA);	//walls in fog
	init_pair(11, COLOR_WHITE,   COLOR_BLACK);		//	
	init_pair(12, COLOR_WHITE,   COLOR_RED);		//	
	init_pair(13, COLOR_BLACK,   COLOR_BLACK);		//
	init_pair(14, COLOR_BLACK,   COLOR_WHITE);		//
	init_pair(15, COLOR_WHITE,   COLOR_MAGENTA);	
	init_pair(16, COLOR_WHITE,   COLOR_GREEN);	

	init_pair(20, COLOR_WHITE,   COLOR_BLUE);		//
	init_pair(21, COLOR_WHITE,   COLOR_RED);		//
	init_pair(22, COLOR_WHITE,   COLOR_YELLOW);		//
	init_pair(23, COLOR_WHITE,   COLOR_CYAN);		//
    }
	return 0;
}

  int get_input() {	return getch();	}
  
  void NC_clear() { clear();	}
  void NC_color_set(int c) {color_set(c, NULL);}
  void NC_mvaddch(int y, int x, char c) {mvaddch(y,x, c);}
  void NC_mvaddstr( int y, int x, std::string str) {mvaddstr(y,x,str.c_str());}
  void NC_move(int y, int x) {move(y,x);}
  void NC_refresh() {refresh();}
  void NC_deleteln() {deleteln();}

  int exit() {
  /*  Clean up after ourselves  */

    delwin(mainwin);
    endwin();
    refresh();
  	return 0;
  }


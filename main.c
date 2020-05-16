/**
 * Termirain.
 * An animated screensaver for *Nix TUIs.
 * 
 * Copyright (C) 2020 Jesse Phillips, Released under the GNU GPL v3.0 or later.
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.  This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  
 * You should have received a copy of the GNU General Public License along with this program.  
 * If not, see <http://www.gnu.org/licenses/>.
**/
/**
 * main.c - the main controller for termirain.
 * @author Jesse Phillips
 **/
#include <stdio.h>/* I/O Library */
#include <stdlib.h>/* Standard Library */
#include <stdbool.h>/* Boolean Library */
#include <ncurses.h>/* NCurses (TUI) Library */
#include <unistd.h>/* Used for usleep() */
#include <time.h>/* Time Library */
#define AMOUNTOFRAIN 50/* The number of raindrops. */

/**
 *  Defines a raindrop.
 **/
typedef struct raindrop
{
  short yLocation;/* Y co-ordinate. */
  short xLocation;/* X co-ordinate. */
}raindrop;

raindrop raindrops[AMOUNTOFRAIN];/* Contains an array of each raindrop. */
WINDOW *sky;/* This will form the sky above the cities, the majority of the game will happen here. */
WINDOW *landscape;/* This will form the landscape at the bottom of the screen, including cities. */

/**
 *  Builds an NCurses window on the screen, with a given size and location. 
 * @param window - A pointer to the window to build.
 * @param height - The window height.
 * @param width - The window width
 * @param yLocation - The Y-axis location of the window on the screen.
 * @param xLocation - The X-axis location of the window on the screen.
 **/
void createWindow(WINDOW **window,int height, int width, int yLocation, int xLocation)
{
  *window = newwin(height, width, yLocation, xLocation);
  wrefresh(*window);
}

/**
 * Replaces the OS terminal colour scheme with our own, and declares the colour pairs we'll need.
 **/
void prepareScreen()
{
  init_pair(1, COLOR_BLACK, COLOR_BLUE);/* Set the first colour pair to black-on-blue. */
  init_pair(2, COLOR_BLACK, COLOR_BLACK);/* Set black-on-black colour scheme. */
  init_pair(3, COLOR_BLACK, COLOR_GREEN);/* Set black-on-green colour scheme. */
  
  /* Clear sky to pair 1 */
  wattron(sky, COLOR_PAIR(2));
  for (short yAxis = 0; yAxis < LINES - 3; yAxis++)
  {
    for (short xAxis = 0; xAxis < COLS; xAxis++)
      mvwaddch(sky, yAxis, xAxis, ' ');/* Fill every character on the sky window with a black space. */
  }
  wrefresh(sky);
  wattroff(sky, COLOR_PAIR(2));
  wattron(sky, COLOR_PAIR(1));/* Initialise Black-on-black colour scheme. */

  /* Clear landscape to pair 3 */
  wattron(landscape, COLOR_PAIR(3));/* Initialise Black-on-green colour scheme. */
  for (short yaxis = 0; yaxis < 3; yaxis++)
  {
    for (short xaxis = 0; xaxis < COLS; xaxis++)
      mvwaddch(landscape, yaxis, xaxis, ' ');/* Fill every character on the landscape window with a green space. */
  }
  wrefresh(landscape);
}

/** 
 * Initialises NCurses. 
 **/
void initNCurses()
{
  initscr();/* Initialise the screen for NCurses. */
  if (!has_colors())
  {
    endwin();
    printf("\e[1;1H\e[2JNo Colour Support Found.\n");
    exit(EXIT_FAILURE);/* Fail if there's no colour support.  I might add mono support in a future version. */
  }
  cbreak();/* Enable char-by-char input mode. */
  noecho();/* Don't output user input direct to the screen.*/
  curs_set(0);/* Make the default cursor invisible. */
  keypad(stdscr, TRUE);/* Accept non-alphanumeric key inputs. */
  createWindow(&sky, LINES - 3, COLS, 0, 0);
  createWindow(&landscape, 3, COLS, LINES - 3, 0);
  start_color();/* Enable colour support in NCurses. */
  prepareScreen();
}

/**
 * Time-based Pseudo-random number generation. 
 * @param min - The minimum possible number to choose from.
 * @param max - The maximum possible number to choose from.
 **/
short rNG(short min, short max)
{
  return (rand() % (max - min + 1)) + min;
}

/** 
 * Handles the animation and escape conditions.
 **/
void makeItRain()
{
  /* Set the x- and y-axis co-ordinates of each raindrop. */
  for (short drop = 0; drop < AMOUNTOFRAIN; drop++)
  {
    raindrops[drop].xLocation = rNG(0, COLS);
    raindrops[drop].yLocation = rNG(0, LINES - 3);
  }
  while (true)
  {
    wtimeout(sky, 0);/* Set any input checks in the window as non-blocking. */
    usleep(100000);/* Update the screen every x microseconds by suspending this thread. */

    for (short drop = 0; drop < AMOUNTOFRAIN; drop++)
    {
      mvwaddch(sky, raindrops[drop].yLocation, raindrops[drop].xLocation, ' ');/* Draw a raindrop. */

      /* Remove the raindrop from it's previous location. */
      wattroff(sky, COLOR_PAIR(1));
      wattron(sky, COLOR_PAIR(2));
      if (raindrops[drop].yLocation == 0)
        mvwaddch(sky, LINES - 4, raindrops[drop].xLocation, ' ');
      else
        mvwaddch(sky, raindrops[drop].yLocation - 1, raindrops[drop].xLocation, ' ');
      wattroff(sky, COLOR_PAIR(2));
      wattron(sky, COLOR_PAIR(1));

      if (raindrops[drop].yLocation < LINES - 4)
        raindrops[drop].yLocation++;/* If the raindrop isn't at the bottom of the window, move it down one pixel. */
      else
        raindrops[drop].yLocation = 0;/* Move it to the top of the window to begin again. */
    }
    wrefresh(sky);/* Refresh the window graphics. */

    switch (wgetch(sky))
    {
      case ' ':/* When the user hits [space], kill the animation. */
        return;
      default:/* Ignore any other input. */
        break;
    }
  }
}

/** 
 * Deletes all of the NCurses windows, given their names.
 **/
void killNCurses()
{
  delwin(sky);
  delwin(landscape);
  endwin();/* Close NCurses */
  printf("\e[1;1H\e[2JThank you for using Termirain.  Goodbye.\n");/* Output the Bash keycode to clear the screen, followed by a goodbye message. */
}

/**
 * Main function, called by the OS.  
 * Void out any args because we aren't using them, so why waste the memory storing them? 
 **/
int main(void)
{
  srand((unsigned) time(NULL));/* Used in rNG() for time-based pseudo-random number generation. */
  initNCurses();/* Initialise NCurses windows. */
  makeItRain();/* Does what it says on the tin. */
  killNCurses();/* Safely kill all the NCurses windows and end NCurses. */
  return EXIT_SUCCESS;/* Close the program with a success signal. */
}
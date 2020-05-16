# termirain

_An animated screensaver for *Nix TUIs._
Termirain is an animated screensaver (using NCurses) for *Nix Terminals, displaying a screenful of rain pouring onto a field.

## Install

- Download Termirain and navigate to the directory containing the download.
- _As root_ run either:`make` or `gcc -std=gnu11 -o /usr/bin/rain main.c -lncurses` in this directory.
- Or _as a user with sudo privileges_ run either:`sudo make` or `sudo gcc -std=gnu11 -o /usr/bin/rain main.c -lncurses` in this directory.

## Run

To run Termirain, just type `rain` into your terminal.

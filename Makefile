.DEFAULT_GOAL := mini-clutter-wm

PREFIX=/usr/local

# clutter includes most stuff needed
FLAGS=`pkg-config --cflags --libs x11 clutter-1.0` 

mini-clutter-wm: mini-clutter-wm.c
	gcc mini-clutter-wm.c -o mini-clutter-wm $(FLAGS) 

install:
	mkdir -p $(PREFIX)/bin
	install mini-clutter-wm $(PREFIX)/bin/

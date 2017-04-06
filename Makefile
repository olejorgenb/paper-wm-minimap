.DEFAULT_GOAL := paper-wm-minimap

PREFIX=/usr/local

# clutter includes most stuff needed
FLAGS=`pkg-config --cflags --libs x11 clutter-glx-1.0` 

paper-wm-minimap: paper-wm-minimap.c
	gcc paper-wm-minimap.c -o paper-wm-minimap $(FLAGS) 

install:
	mkdir -p $(PREFIX)/bin
	install paper-wm-minimap $(PREFIX)/bin/

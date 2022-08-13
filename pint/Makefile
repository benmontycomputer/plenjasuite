all: pint

pint:
	cc pint.c -lncursesw -std=c11 -lplenja $(ncursesw6-config --cflags --libs) -o bin/pint

debug:
	cc pint.c -lncursesw -std=c11 -lplenja $(ncursesw6-config --cflags --libs) -o bin/pint -g

install:
	@./install.sh

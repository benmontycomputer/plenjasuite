all: libplenja

libplenja:
	gcc -fPIC fs/libplenja-fs.c -o fs/libplenja-fs.o -c
	gcc -fPIC str/libplenja-str.c -o str/libplenja-str.o -c
	gcc fs/libplenja-fs.o str/libplenja-str.o -shared -o libplenja.so
	echo '' | cat fs/libplenja-fs.h - str/libplenja-str.h > libplenja.h

install:
	if [ -f /usr/lib/libplenja.so ]; then
		rm /usr/lib/libplenja.so
	fi
	if [ -f /usr/lib/libplenja.so.0 ]; then
		rm /usr/lib/libplenja.so.0
	fi
	if [ -f /usr/lib/libplenja.so.0.0.0 ]; then
		rm /usr/lib/libplenja.so.0.0.0
	fi
	cp libplenja.so /usr/lib/libplenja.so.0.0.0
	ln -s libplenja.so.0.0.0 /usr/lib/libplenja.so.0
	ln -s libplenja.so.0.0.0 /usr/lib/libplenja.so

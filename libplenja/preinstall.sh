#!/bin/sh

if [ -f /usr/lib/libplenja.so ]; then
	rm /usr/lib/libplenja.so
fi
if [ -f /usr/lib/libplenja.so.0 ]; then
	rm /usr/lib/libplenja.so.0
fi
if [ -f /usr/lib/libplenja.so.0.0.0 ]; then
	rm /usr/lib/libplenja.so.0.0.0
fi

if [ -f /usr/include/libplenja.h ]; then
	rm /usr/include/libplenja.h
fi


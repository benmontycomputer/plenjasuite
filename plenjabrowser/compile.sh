#!/bin/sh

cython -3 main.py --embed
PYTHONLIBVER=python$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')$(python3-config --abiflags)
gcc -Os $(python3-config --includes) main.c -o bin/plenjabrowser $(python3-config --ldflags) -l$PYTHONLIBVER

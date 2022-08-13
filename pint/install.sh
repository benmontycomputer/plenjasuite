#!/bin/sh

if [ "$PREFIX" == "" ]; then
    cp -f bin/pint /usr/bin/
else
    cp -f bin/pint ${PREFIX}/bin/
fi
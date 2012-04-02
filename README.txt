Manic Miner

This is a clone of a game called Manic Miner, written by Matthew Smith and
published in 1983 by Bug Byte. I wrote this in C using the cc65 cross-compiler
toolchain, targeting the Commodore 64.

If you have make and cc65 installed, you should be able to type make and have
the project build.

The Makefile requires a version of cc65 that supports dependency generation. I
am currently using a nightly build from February 2012.

cc65 website: http://www.cc65.org/
Makefile website: http://wiki.cc65.org/doku.php?id=cc65:project_setup

The Makefile has these credits:
###############################################################################
### Generic Makefile for cc65 projects - full version with abstract options ###
### V1.2.1(w) 2010 - 2011 Oliver Schmidt & Patryk "Silver Dream !" Łogiewa  ###
###############################################################################

cc65 (cl65) has these credits:
cl65 V2.13.9 - (C) Copyright 1998-2011 Ullrich von Bassewitz

If you don't have make or you are using an older version of cc65 then the
following command should work just fine: 
cl65 -Oi src/globals.c src/data.c src/render.c src/game.c -o manicminer.c64 -C src/manic64.cfg

The file manicminer.c64 is the file you would want to load.

If you have andy feedback, or fix any bugs, please email me at
swessels@email.com. Any and all feedback welcome!

Thank you
Stefan Wessels, December 2010. 
(updated README.txt April 2, 2012)

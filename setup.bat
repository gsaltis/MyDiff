@echo off
doskey m=make -j 4 -f Makefile.Release $T make -j 4 -f Makefile.Debug
doskey mr=color $T cls $T make -j 4 -f Makefile.Release
doskey md=make -j 4 -f Makefile.Debug
doskey mc=make cleanall
doskey mq=qmake
doskey my=release\mydiff $*
doskey myf=release\mydiff -1 Test\Track2Main.c -2 Test\Track3Main.c $*
doskey myd=debug\mydiff $*
title MyDiff

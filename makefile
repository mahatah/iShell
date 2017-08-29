# This is the makefile for iShell. Files needed are iShell.h, iShell_buld.c
# and iShell_lex.yy.
CC=cc
LEX=flex

all: iShell

iShell: lex.yy.o
	cc lex.yy.o -o iShell
	rm -rf *.0 lex.yy.c lex.yy.o

lex.yy.c: iShell_lex.yy
	flex iShell_lex.yy

lex.yy.o: lex.yy.c

clean:
	rm -rf *.o lex.yy.c 

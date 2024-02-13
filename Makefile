all:
	cc -o main main.c -lX11 -lGL -lGLU

run: all
	./main

.phony:
	run

all:
	cc main.c -o main -lglfw -lGLEW -lGL -lm

run: all
	./main

.phony:
	run

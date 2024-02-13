all:
	cc main.c -o main -lglfw -lGL

run: all
	./main

.phony:
	run

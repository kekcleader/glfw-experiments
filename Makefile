PROG=main2

all:
	cc $(PROG).c -o $(PROG) -lglfw -lGLEW -lGL -lm

run: all
	./$(PROG)

.phony:
	run

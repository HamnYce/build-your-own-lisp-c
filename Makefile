all: compile run

compile:
	cc -std=c99 -Wall prompt.c mpc.c -ledit -lm -o ./build/prompt

run:
	./build/prompt

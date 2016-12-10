all:
	cc -o snake snake.c PCD8544.c  -L/usr/local/lib -lwiringPi -lcurses
main: main.c othutility.c
	gcc main.c othutility.c -o main -lncurses

clean:
	rm main

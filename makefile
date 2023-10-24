test : main.c
	gcc -Wall main.c
	./a.out
build-sanitize : main.c
	gcc -g3 -fsanitize=address,undefined main.c
build : main.c
	gcc -g -Wall main.c
clean : a.out
	rm a.out
build-pedantic: main.c
	gcc -pedantic main.c
run : ./a.out
	./a.out

test : main.c
	gcc -Wall main.c
	./a.out
sanitize : main.c
	gcc -g3 -fsanitize=address,undefined main.c
	./a.out
build : main.c
	gcc -g3 -fsanitize=address,undefined main.c
run : a.out
	./a.out
clean : a.out
	rm a.out
pedantic: main.c
	gcc -pedantic main.c
	./a.out

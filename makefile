test : sanitize run_input
sanitize : main.c
	gcc -g3 -fsanitize=address,undefined main.c
build : main.c
	gcc -Wall main.c
run : a.out
	./a.out
run_input: input a.out
	./a.out < input
clean : a.out
	rm a.out
pedantic: main.c 
	gcc -pedantic main.c

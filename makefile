test : main.c
	gcc -g main.c
	./a.out
build : main.c
	gcc -g main.c
run : a.out
	./a.out
clean : a.out
	rm a.out

all:
	gcc -o virtualMem virtualMem.c
	./virtualMem -k 50 -q 10 -m 300 -i 400

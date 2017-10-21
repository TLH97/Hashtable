hash_table: hash_table.o hash.o
	gcc hash_table.o hash.o -o hash_table

hash_table.o: hash_table.c
	gcc -c -Wall hash_table.c

hash.o: hash.c
	gcc -c -Wall hash.c

build: hash_table 

run: hash_table
	./hash_table

clean:
	rm -rf hash_table hash_table.o hash.o

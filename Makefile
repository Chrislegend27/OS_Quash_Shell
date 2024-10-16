shell2: shell2.c
	gcc -o shell2 shell2.c -I.

shell: shell.c
	gcc -std=gnu99 -o shell shell.c
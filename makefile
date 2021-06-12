ex14 : check.o dataUpdate.o checkFunc.o parsing.o main.o
	gcc -g -ansi -Wall -pedantic check.o dataUpdate.o checkFunc.o parsing.o main.o -o ex14

main.o:	main.c global.h
	gcc -c -ansi -Wall -pedantic main.c -o main.o
check.o: check.c global.h extern.h
	gcc -c -ansi -Wall -pedantic check.c -o check.o
dataUpdate.o: dataUpdate.c global.h extern.h
	gcc -c -ansi -Wall -pedantic dataUpdate.c -o dataUpdate.o
checkFunc.o: checkFunc.c global.h extern.h
	gcc -c -ansi -Wall -pedantic checkFunc.c -o checkFunc.o
parsing.o: parsing.c global.h extern.h
	gcc -c -ansi -Wall -pedantic parsing.c -o parsing.o
	
clean:
	rm *.o

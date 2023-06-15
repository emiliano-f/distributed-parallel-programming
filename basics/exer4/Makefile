generalpaz: common.o structs.o queue.o checkout.o updater.o main.o
	@echo Generating executablo generalpaz
	gcc common.o structs.o queue.o checkout.o updater.o main.o -o generalpaz


main.o: main.c common.h structs.h queue.h updater.h checkout.h
	@echo main.o
	gcc -c main.c

updater.o: updater.c common.h structs.h queue.h 
	@echo updater.o
	gcc -c updater.c

checkout.o: checkout.c common.h structs.h queue.h
	@echo checkout.o
	gcc -c checkout.c

queue.o: queue.c common.h structs.h
	@echo queue.o
	gcc -c queue.c

structs.o: structs.c common.h
	@echo structs.o
	gcc -c structs.c

common.o: common.c
	@echo common.o
	gcc -c common.c


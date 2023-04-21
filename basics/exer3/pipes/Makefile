sieteymedio: creation.o croupier.o player.o
	@echo Generating executable sieteymedio
	gcc creation.o croupier.o player.o -o sieteymedio

creation.o: creation.c response.h player.h croupier.h
	@echo creation.o
	gcc -c creation.c

player.o: player.c
	@echo player.o
	gcc -c player.c

croupier.o: croupier.c
	@echo croupier.o
	gcc -c croupier.c

response.o: response.c
	@echo response.o
	gcc -c response.c

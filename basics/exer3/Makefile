# No se puede hacer un ejecutable con dos main
sieteymedio: creation.o croupier.o player.o
	@echo Generating executable sieteymedio
	gcc creation.o croupier.o player.o -o sieteymedio

creation.o: creation.c response.h player.h croupier.h
	@echo creation.o
	gcc -c creation.c

#player.h: player.c
#@echo player.h
#	cpp -dM player.c > player.h

player.o: player.c
	@echo player.o
	gcc -c player.c

#croupier.h: croupier.c
#	@echo croupier.h
#	cpp -dM croupier.c > croupier.h

croupier.o: croupier.c
	@echo croupier.o
	gcc -c croupier.c

#response.h: response.c
#	@echo response.h
#	cpp -dM response.c > response.h

response.o: response.c
	@echo response.o
	gcc -c response.c

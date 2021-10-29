CFLAGS= -Wall -O2

exec: stun.o example.o 
	cc $(CFLAGS) -o Exec stun.o example.o 

clean:
	rm *.o Exec 

example.o: example.c
stun.o: stun.c stun.h




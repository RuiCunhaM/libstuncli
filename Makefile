# Flags Normais 
CFLAGS= -Wall -O2

#Flags para profiling 
#CFLAGS= -Wall -O2 -pg  

# Compiles
exec: stun.o  
	cc $(CFLAGS) -o Exec stun.o 

# Clean
clean:
	rm *.o Exec profile gmon.out 

# Files
stun.o: stun.c




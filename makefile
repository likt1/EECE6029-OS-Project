CC=gcc
CFLAGS=-I.
DEPS = jobs.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: jobs.o schedule_sim.o 
	g++ -o schedule_sim.out schedule_sim.o jobs.o -I.
	
clean:
	rm -f *.o *.out


CC=gcc
CFLAGS=-I.
DEPS = jobs.h

CXXFLAGS=-std=c++11

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: jobs.o scheduler.o schedule_sim.o 
	g++ -o schedule_sim.out schedule_sim.o jobs.o scheduler.o -I.
	
clean:
	rm -f *.o *.out


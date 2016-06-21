OBJS = main.o effects.o
PROGRAM = main 

# Important optimization options
CFLAGS = -O3 -ffast-math -fno-rtti

# Standard libraries
LFLAGS = -lm -lstdc++ -lpthread -lrtmidi

# Debugging
CFLAGS += -g -Wall
LFLAGS += -g

# Annoying warnings on by default on Mac OS
CFLAGS += -Wno-tautological-constant-out-of-range-compare -Wno-gnu-static-float-init

all: $(OBJS)
	$(CC) $(OBJS) -o $(PROGRAM) $(LFLAGS)

-include $(OBJS:.o=.d)

%.o: %.cpp
	$(CC) -c $(DEBUG) $(CFLAGS) $*.cpp -o $*.o
	$(CC) -MM $(DEBUG) $(CFLAGS) $*.cpp > $*.d

clean:
	$(RM) $(PROGRAM) *.o

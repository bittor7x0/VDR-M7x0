GAME     = minesweeper
OBJS     = minesweeper.o
CFLAGS   = -g -pipe -O2 -Wall -Woverloaded-virtual

all: $(OBJS)
	@ar -r $(GAME).a $(OBJS)

clean:
	@echo -n "."
	@rm -rf *.o *.a

.cpp.o:
	@echo -n "."
	@g++ $(CFLAGS) -c $<

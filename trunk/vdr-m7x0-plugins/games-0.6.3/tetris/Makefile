GAME     = tetris
OBJS     = tetris.o
CFLAGS   = -fPIC -g -pipe -O2 -Wall -Woverloaded-virtual

all: $(OBJS)
	@$(AR) -r $(GAME).a $(OBJS)

clean:
	@echo -n "."
	@rm -rf *.o *.a

.cpp.o:
	@echo -n "."
	$(CXX) $(CFLAGS) -c $<

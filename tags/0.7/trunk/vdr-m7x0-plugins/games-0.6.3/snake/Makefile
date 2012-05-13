GAME     = snake
OBJS     = snake.o
CFLAGS   += -pipe

all: $(OBJS)
	@$(AR) -r $(GAME).a $(OBJS)

clean:
	@echo -n "."
	@rm -rf *.o *.a

.cpp.o:
	@echo -n "."
	$(CXX) $(CFLAGS) -c $<

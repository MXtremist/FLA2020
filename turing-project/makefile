CC = g++
CFLAGS = -c
OBJS = main.o simulator.o parser.o terminal.o

turing:$(OBJS)
	$(CC) -o turing $(OBJS)
	rm -rf *.o
	
%.o:%.c terminal.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	@rm -rf *.o turing
	@echo "Clean done!"
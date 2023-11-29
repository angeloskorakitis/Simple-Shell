# compiler
CC = g++

# paths
SRC = ../src
INCLUDE = ../include

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -g -I$(INCLUDE)
LDFLAGS = -lpthread

# Αρχεία .o
OBJS = main.o 
OBJS += utils.o parser.o builtins.o

# Το εκτελέσιμο πρόγραμμα
EXEC = mysh

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(CFLAGS) $(LDFLAGS)

run: $(EXEC)
	./$(EXEC) 

valgrind: $(EXEC)
	valgrind ./$(EXEC) 

clean:
	rm -f $(OBJS) $(EXEC)
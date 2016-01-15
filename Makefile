OBJS = raycast.cpp
CC = g++
COMPILER_FLAGS = -Wall -O3
LINKER_FLAGS = -lSDL2
OBJ_NAME = raycast

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
	./$(OBJ_NAME)
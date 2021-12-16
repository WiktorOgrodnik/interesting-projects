CC = gcc
SRC_DIR = .
INC_DIR = .
CFLAGS = -std=c18 -Wall -Wextra -O2 -Wno-unused-parameter
NAME = nspline
LIBS = -lm

OBJS = main.o image.o

all: $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LIBS)

%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c -I$(INC_DIR) $< -o $@ $(LIBS)

clean:
	@rm -f $(NAME) $(OBJS)
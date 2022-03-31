
# Wiktor Ogrodnik
# 323129

CC = gcc
SRC_DIR = src
INC_DIR = .
OBJ_DIR = obj
CFLAGS = -std=gnu18 -Wall -Wextra -Wpedantic -O2
NAME = traceroute
OBJS = $(addprefix $(OBJ_DIR)/, main.o icmp.o)

all: $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c -I$(INC_DIR) $< -o $@ $(GTK)

clean:
	@rm -f $(OBJS)

distclean:
	@rm -f $(NAME) $(OBJS)
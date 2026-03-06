# Fill this makefile with the makefile format instructions to 
#   build your program.

# Makefile template from W2.3
# Define C compiler & flags
CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g

# Define libraries to be linked (for example -lm)
LDLIBS = 

# Common source files reused by Stage 1 and Stage 2
COMMON_SRC = main.c csv_loader.c linked_list.c records.c bit.c dictionary.c
COMMON_OBJ = $(COMMON_SRC:.c=.o)
 
# Stage 1 (linked list)
S1_SRC = dictionary.c
S1_OBJ = $(S1_SRC:.c=.o)
# Define the executable name(s)
EXE1 = dict1

# Stage 2 (Patricia tree)
S2_SRC = radix_tree.c
S2_OBJ = $(S2_SRC:.c=.o)
# Define the executable name(s)
EXE2 = dict2

.PHONY: all
all: $(EXE1) $(EXE2)

$(EXE1): $(COMMON_OBJ) $(S1_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(EXE2): $(COMMON_OBJ) $(S2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Other targets specify what .o files rely on
main.o:       main.c dictionary.h csv_loader.h records.h linked_list.h bit.h radix_tree.h
dictionary.o: dictionary.c dictionary.h linked_list.h records.h bit.h
linked_list.o: linked_list.c linked_list.h records.h
records.o:    records.c records.h
csv_loader.o: csv_loader.c csv_loader.h records.h dictionary.h linked_list.h bit.h
bit.o:        bit.c bit.h
radix_tree.o: radix_tree.c radix_tree.h records.h bit.h

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f *.o $(EXE1) $(EXE2)



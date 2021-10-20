CXX := gcc
FLAGS := -Wextra -Wall -g
target := comparator.out
objs := $(patsubst %.c,%.o,$(wildcard *.c))
AARCH := dumpfiles/aarch64_coremark.dump
RICSV := dumpfiles/riscv64_coremark.dump
.PHONY: all clean run

all: $(target)
%.o: %.c %.h
	$(CXX) $(FLAGS) -c $<
main.o: %.o: %.c core.h
	$(CXX) $(FLAGS) -c $<
$(target): $(objs)
	$(CXX) $(FLAGS) -o $@ $^
run:
	./$(target) $(RICSV) $(AARCH)
clean:
	rm -f *.o $(target)

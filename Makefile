CXX := gcc
FLAGS := -Wextra -Wall -g
target := comparator.out
objs := $(patsubst %.c,%.o,$(wildcard *.c))
AARCH := dumpfiles/aarch64_coremark.dump
RISCV := dumpfiles/riscv64_coremark.dump
subdir := subroutines
bbdir := bbroutines/a bbroutines/r bbroutines
.PHONY: all clean run

all: $(target)
%.o: %.c %.h
	$(CXX) $(FLAGS) -c $<
main.o: %.o: %.c core.h
	$(CXX) $(FLAGS) -c $<
$(target): $(objs)
	$(CXX) $(FLAGS) -o $@ $^
run: all
	mkdir $(subdir) -p
	mkdir $(bbdir) -p
	./$(target) $(RISCV) $(AARCH)
clean:
	rm -rf $(subdir)
	rm -rf $(bbdir)
	rm -f *.o $(target)

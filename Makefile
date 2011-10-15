#include /Users/fable/Projects/stxxl/stxxl.mk
include ../stxxl-1.3.1/stxxl.mk


all: test1 test2 test3

test1: test1.cpp
	$(STXXL_CXX) -c test1.cpp -g $(STXXL_CPPFLAGS)
	$(STXXL_CXX) test1.o -o test1.bin $(STXXL_LDLIBS)

test2: test2.cpp
	$(STXXL_CXX) -c test2.cpp -g $(STXXL_CPPFLAGS)
	$(STXXL_CXX) test2.o -o test2.bin $(STXXL_LDLIBS)

test3: test3.cpp
	$(STXXL_CXX) -c test3.cpp -g $(STXXL_CPPFLAGS)
	$(STXXL_CXX) test3.o -o test3.bin $(STXXL_LDLIBS)

clean:
	rm -f test1.o test1.bin
	rm -f test2.o test2.bin
	rm -f test3.o test3.bin

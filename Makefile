CC=g++
CFLAGS=-pedantic-errors -g
SOURCES_EXECUTABLE=boolcalc.cpp boolexpr.cpp
SOURCES_TEST=test.cpp boolexpr.cpp

all: boolcalc test
    
boolcalc:
	$(CC) $(SOURCES_EXECUTABLE) $(CFLAGS) -o $@

test: 
	$(CC) $(SOURCES_TEST) $(CFLAGS) -lgtest -lgtest_main -lpthread -o $@

.PHONY: clean

clean:
	rm -rf *.o boolcalc test

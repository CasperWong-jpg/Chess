SOURCES := src/*.c
HEADERS := src/*.h

OUTPUTDIR := bin

COMPILER := /usr/bin/clang
CFLAGS := -std=c17
DEBUGFLAGS := -Wall -Wextra -Werror -Wshadow -std=c99 -pedantic -g -fwrapv -DDEBUG=0

all: playable

# Create shared object file that can be called by Python function
lichess: $(SOURCES) $(HEADERS)
	mkdir -p $(OUTPUTDIR)
	$(COMPILER) -o $(OUTPUTDIR)/ChessEngine.so -fPIC -shared $(CFLAGS) $(SOURCES)

# Create command line executable to simulate gameplay
playable: $(SOURCES) $(HEADERS)
	mkdir -p $(OUTPUTDIR)
	$(COMPILER) -o $(OUTPUTDIR)/ChessEngine.o $(CFLAGS) $(DEBUGFLAGS) $(SOURCES)

clean:
	rm -rf $(OUTPUTDIR)

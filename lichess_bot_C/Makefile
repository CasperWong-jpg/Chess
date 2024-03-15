SOURCES := src/*.c src/lib/libomp.dylib
HEADERS := src/*.h

OUTPUTDIR := bin
LICHESSDIR := lichess_bot/engines

COMPILER := /usr/bin/clang
CFLAGS := -std=c17
LDFLAGS := -I./src/include/
DEBUGFLAGS := -Wall -Wextra -Werror -Wshadow -std=c99 -g -fwrapv # Wpedantic <-- this is too picky for me

all: playable

# Create shared object file that can be called by Python function
lichess: $(SOURCES) $(HEADERS)
	mkdir -p $(OUTPUTDIR)
	$(COMPILER) -o $(LICHESSDIR)/ChessEngine.so -fPIC -shared $(CFLAGS) $(LDFLAGS) $(SOURCES)

# Create command line executable to simulate gameplay
playable: $(SOURCES) $(HEADERS)
	mkdir -p $(OUTPUTDIR)
	$(COMPILER) -o $(OUTPUTDIR)/ChessEngine.o $(CFLAGS) $(LDFLAGS) $(DEBUGFLAGS) $(SOURCES)

clean:
	rm -rf $(OUTPUTDIR)
	rm -f $(LICHESSDIR)/ChessEngine.so
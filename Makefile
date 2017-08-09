CFILES := src/main.c src/algos.c src/sqlite3.c
LIBS   := -lssl -lcrypto -lpthread -ldl
OUTPUT := hasher

all:
	gcc -Wall $(CFILES) -o $(OUTPUT)  $(LIBS)

debug:
	gcc -Wall $(CFILES) -g -o $(OUTPUT) $(LIBS)

clean:
	rm -f $(OUTPUT)

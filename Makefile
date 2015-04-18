CFLAGS := -std=c99 -lm -g -I.

bin/dot3k-test: test/dot3k-test.c libdot3k.a 
	$(CC) $(CFLAGS) -o $@ $< libdot3k.a

bin/midi-test: test/midi-test.c libdot3k.a 
	$(CC) $(CFLAGS) -o $@ $< libdot3k.a -lportmidi

libdot3k.a: lib/backlight.o lib/dot3k.o
	ar ru $@ $?
	ranlib $@

all: bin/dot3k-test bin/midi-test

clean:
	rm lib/*.o
	rm libdot3k.a
	rm bin/*

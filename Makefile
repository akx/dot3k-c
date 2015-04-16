CFLAGS := -std=c99 -lm

.phony: dot3k-test

libdot3k.a: lib/backlight.o lib/dot3k.o
	ar ru $@ $?
	ranlib $@

dot3k-test: dot3k-test.c libdot3k.a 

clean:
	rm lib/*.o
	rm libdot3k.a

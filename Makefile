CFLAGS := -std=c99 -lm -g

dot3k-test: dot3k-test.c libdot3k.a 

libdot3k.a: lib/backlight.o lib/dot3k.o
	ar ru $@ $?
	ranlib $@


clean:
	rm lib/*.o
	rm libdot3k.a

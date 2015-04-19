CFLAGS := -std=c99 -lm -g -I.

bin/bl-test: test/bl-test.c libdot3k.a 
	$(CC) $(CFLAGS) -o $@ $< libdot3k.a

bin/midi-test: test/midi-test.c libdot3k.a 
	$(CC) $(CFLAGS) -o $@ $< libdot3k.a -lportmidi
	
bin/joy-test: test/joy-test.c libdot3k.a 
	$(CC) $(CFLAGS) -o $@ $< libdot3k.a

libdot3k.a: lib/backlight.o lib/joystick.o lib/dot3k.o lib/rpi_gpio.o
	ar ru $@ $?
	ranlib $@

all: bin/dot3k-test bin/midi-test

clean:
	rm lib/*.o
	rm libdot3k.a
	rm bin/*

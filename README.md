# dot3k-c
* https://github.com/akx/dot3k-c

A C library for interfacing with [Pimoroni's Display-O-Tron 3000](http://shop.pimoroni.com/products/displayotron-3000) LCD module for the Raspberry Pi.

Based on Pimoroni's Python code from

* https://github.com/pimoroni/dot3k
* https://github.com/pimoroni/st7036
* https://github.com/pimoroni/sn3218


# Display-O-Tron HAT Update
* https://github.com/pmprog/dot3k-c

Added a new version which supports [Pimoroni's Display-O-Tron HAT](https://shop.pimoroni.com/collections/raspberry-pi/products/display-o-tron-hat) LCD module for Raspberry Pi.

# Compiling
Build everything

    make [all]
    
Build just the DOT3K library

    make dot3k

Build just the DOTHAT library

    make dothat
    
Clean all build files

    make clean

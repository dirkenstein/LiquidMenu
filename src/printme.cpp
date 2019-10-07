/*
The MIT License (MIT)

Copyright (c) 2016 Vasil Kalchev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
@file
printme helper function*/

#include "LiquidMenu.h"

void print_me(uintptr_t address) {
	DEBUG(F("Line (0x")); DEBUG2(address, OCT); DEBUG(F("): "));
	return;
	address = address;
}

uint8_t * rotTile(uint8_t * tile) {
    uint8_t * newt = new uint8_t[8];
    for (int x = 0; x < 8; x++) {
        uint8_t newb = 0;
        for (int y = 0 ; y < 8; y++){
            newb |= (tile[y] << x) & 0x80;
            newb >>= 1;
        }
        newt[x] = newb;
    }
    return newt;
}

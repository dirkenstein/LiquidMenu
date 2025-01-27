/**
@file
Contains the LiquidScreen class definition.
*/

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

#include "LiquidMenu.h"
#pragma once

template <class Disp>
LiquidScreen<Disp>::LiquidScreen()
	: _lineCount(0), _focus(0), _hidden(false) {}

template <class Disp>
LiquidScreen<Disp>::LiquidScreen(LiquidLine<Disp> &liquidLine)
	: LiquidScreen() {
	add_line(liquidLine);
}
template <class Disp>
LiquidScreen<Disp>::LiquidScreen(LiquidLine<Disp> &liquidLine1, LiquidLine<Disp> &liquidLine2)
	: LiquidScreen(liquidLine1) {
	add_line(liquidLine2);
}

template <class Disp>
LiquidScreen<Disp>::LiquidScreen(LiquidLine<Disp> &liquidLine1, LiquidLine<Disp> &liquidLine2,
                           LiquidLine<Disp> &liquidLine3)
	: LiquidScreen(liquidLine1, liquidLine2) {
	add_line(liquidLine3);
}

template <class Disp>
LiquidScreen<Disp>::LiquidScreen(LiquidLine<Disp> &liquidLine1, LiquidLine<Disp> &liquidLine2,
                           LiquidLine<Disp> &liquidLine3, LiquidLine<Disp> &liquidLine4)
	: LiquidScreen(liquidLine1, liquidLine2, liquidLine3) {
	add_line(liquidLine4);
}

template <class Disp>
bool LiquidScreen<Disp>::add_line(LiquidLine<Disp> &liquidLine) {
	print_me(reinterpret_cast<uintptr_t>(this));
	if (_lineCount < MAX_LINES) {
		_p_liquidLine[_lineCount] = &liquidLine;
		DEBUG(F("Added a new line (")); DEBUG(_lineCount); DEBUGLN(F(")"));
		_lineCount++;
		_focus++;
		// Naively set the number of lines the display has to the
		// number of added LiquidLine objects. When adding more
        // LiquidLine objects that the display's number of lines,
        // void LiquidScreen::set_displayLineCount(uint8_t lines)
        // must be used to set the number of lines the display has.
	    _displayLineCount = _lineCount;
		return true;
	}
	DEBUG(F("Adding line ")); DEBUG(_lineCount);
	DEBUG(F(" failed, edit LiquidMenu_config.h to allow for more lines"));
	return false;
}

template <class Disp>
bool LiquidScreen<Disp>::set_focusPosition(Position position) {
	print_me(reinterpret_cast<uintptr_t>(this));
	if (position == Position::CUSTOM) {
		DEBUGLN(F("Can't set focus position to 'CUSTOM' for the whole screen at once"));
		return false;
	} else {
		DEBUG(F("Focus position set to : ")); DEBUGLN((uint8_t)position);
		for (uint8_t l = 0; l < _lineCount; l++) {
			_p_liquidLine[l]->set_focusPosition(position);
		}
		return true;
	}
}

template <class Disp>
void LiquidScreen<Disp>::set_displayLineCount(uint8_t lines)
{
	_displayLineCount = lines;
}

template <class Disp>
void LiquidScreen<Disp>::hide(bool hide) {
	_hidden = hide;
}

template <class Disp>
void LiquidScreen<Disp>::print(Disp *p_liquidCrystal, const uint8_t focusGlyphs[], uint8_t const customFocusGlyphs[][GLYPH_SIZE]) const {
	uint8_t lOffset = 0;
	uint8_t displayLineCount = _displayLineCount;
	if (displayLineCount == 0)
		displayLineCount = _lineCount;
	else if (displayLineCount > _lineCount)
		displayLineCount = _lineCount;
	DEBUG("MaxLine: ");
	DEBUG(displayLineCount);
	DEBUG("\n");

	if (_focus >= displayLineCount)
	{
		lOffset = (_focus - displayLineCount) + 1;
		if ((displayLineCount + lOffset) > _lineCount)
			lOffset = (_lineCount - displayLineCount);
	}
	uint8_t offsetRow = 0;
	for (uint8_t l = lOffset; l < displayLineCount + lOffset; l++) {
		bool focus = true;
		if (_focus != l) {
			focus = false;
			DEBUG(F("|"));
		} else {
			DEBUG(F("|   -->"));
		}
		DEBUG(F("\tLine ")); DEBUG(l);
		if (displayLineCount < _lineCount)
			_p_liquidLine[l]->_row = offsetRow;
		_p_liquidLine[l]->print(p_liquidCrystal, focusGlyphs, customFocusGlyphs, focus);
		offsetRow++;
	}
}

template <class Disp>
void LiquidScreen<Disp>::switch_focus(bool forward) {
	print_me(reinterpret_cast<uintptr_t>(this));
	do {
		if (forward) {
			if (_focus < _lineCount) {
				_focus++;
				if (_focus == _lineCount) {
					break;
				}
			} else {
				_focus = 0;
			}
		} else { //else (forward)
			if (_focus == 0) {
				_focus = _lineCount;
				break;
			} else {
				_focus--;
			}
		} //else (forward)
	} while (_p_liquidLine[_focus]->_focusable == false);
	DEBUG(F("Focus switched to ")); DEBUGLN(_focus);
}

template <class Disp>
bool LiquidScreen<Disp>::call_function(uint8_t number) const {
	if (_focus != _lineCount) {
		return _p_liquidLine[_focus]->call_function(number);
	}
	return false;
}

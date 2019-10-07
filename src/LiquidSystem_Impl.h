/**
@file
Contains the LiquidSystem class definition.
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
LiquidSystem<Disp>::LiquidSystem(uint8_t startingMenu)
	: _menuCount(0), _currentMenu(startingMenu - 1) {

}

template <class Disp>
LiquidSystem<Disp>::LiquidSystem(LiquidMenu<Disp> &liquidMenu1, LiquidMenu<Disp> &liquidMenu2,
                           uint8_t startingMenu)
	: LiquidSystem(startingMenu) {
	add_menu(liquidMenu1);
	add_menu(liquidMenu2);
}

template <class Disp>
LiquidSystem<Disp>::LiquidSystem(LiquidMenu<Disp> &liquidMenu1, LiquidMenu<Disp> &liquidMenu2,
                           LiquidMenu<Disp> &liquidMenu3, uint8_t startingMenu)
	: LiquidSystem(liquidMenu1, liquidMenu2, startingMenu) {
	add_menu(liquidMenu3);
}

template <class Disp>
LiquidSystem<Disp>::LiquidSystem(LiquidMenu<Disp> &liquidMenu1, LiquidMenu<Disp> &liquidMenu2,
                           LiquidMenu<Disp> &liquidMenu3, LiquidMenu<Disp> &liquidMenu4,
                           uint8_t startingMenu)
	: LiquidSystem(liquidMenu1, liquidMenu2, liquidMenu3, startingMenu) {
	add_menu(liquidMenu4);
}

template <class Disp>
bool LiquidSystem<Disp>::add_menu(LiquidMenu<Disp> &liquidMenu) {
	// print_me((uintptr_t)this);
	print_me(reinterpret_cast<uintptr_t>(this));
	if (_menuCount < MAX_MENUS) {
		_p_liquidMenu[_menuCount] = &liquidMenu;
		DEBUG(F("Added a new menu (")); DEBUG(_menuCount); DEBUGLN(F(")"));
		_menuCount++;
		return true;
	}
	DEBUG(F("Adding menu ")); DEBUG(_menuCount);
	DEBUGLN(F(" failed, edit LiquidMenu_config.h to allow for more menus"));
	return false;
}

template <class Disp>
bool LiquidSystem<Disp>::change_menu(LiquidMenu<Disp> &p_liquidMenu) {
	// _p_liquidMenu[_currentMenu]->_p_liquidCrystal->clear();
	for (uint8_t m = 0; m < _menuCount; m++) {
		// if ((uintptr_t)&p_liquidMenu == (uintptr_t) & (*_p_liquidMenu[m])) {
		if (reinterpret_cast<uintptr_t>(&p_liquidMenu) == reinterpret_cast<uintptr_t>(&(*_p_liquidMenu[m]))) {
			_currentMenu = m;
			DEBUG(F("Menu changed to ")); DEBUGLN(_currentMenu);
			update();
			return true;
		}
	}
	// DEBUG(F("Invalid request for menu change to ")); DEBUGLN((uintptr_t)&p_liquidMenu);
	DEBUG(F("Invalid request for menu change to ")); DEBUGLN(reinterpret_cast<intptr_t>(&p_liquidMenu));
	return false;
}

template <class Disp>
LiquidScreen<Disp>* LiquidSystem<Disp>::get_currentScreen() const {
	return _p_liquidMenu[_currentMenu]->get_currentScreen();
}

template <class Disp>
void LiquidSystem<Disp>::next_screen() {
	_p_liquidMenu[_currentMenu]->next_screen();
}

template <class Disp>
void LiquidSystem<Disp>::operator++() {
	next_screen();
}

template <class Disp>
void LiquidSystem<Disp>::operator++(int) {
	next_screen();
}

template <class Disp>
void LiquidSystem<Disp>::previous_screen() {
	_p_liquidMenu[_currentMenu]->previous_screen();
}

template <class Disp>
void LiquidSystem<Disp>::operator--() {
	previous_screen();
}

template <class Disp>
void LiquidSystem<Disp>::operator--(int) {
	previous_screen();
}

template <class Disp>
bool LiquidSystem<Disp>::change_screen(uint8_t number) {
	return _p_liquidMenu[_currentMenu]->change_screen(number);
}

template <class Disp>
bool LiquidSystem<Disp>::change_screen(LiquidScreen<Disp> &p_liquidScreen) {
	return _p_liquidMenu[_currentMenu]->change_screen(p_liquidScreen);
}

template <class Disp>
bool LiquidSystem<Disp>::operator=(uint8_t number) {
	return change_screen(number);
}

template <class Disp>
bool LiquidSystem<Disp>::operator=(LiquidScreen<Disp> &p_liquidScreen) {
	return change_screen(p_liquidScreen);
}

template <class Disp>
void LiquidSystem<Disp>::switch_focus(bool forward) {
	_p_liquidMenu[_currentMenu]->switch_focus(forward);
}

template <class Disp>
bool LiquidSystem<Disp>::set_focusPosition(Position position) {
	return _p_liquidMenu[_currentMenu]->set_focusPosition(position);
}

template <class Disp>
bool LiquidSystem<Disp>::set_focusSymbol(Position position, uint8_t symbol[8]) {
	return _p_liquidMenu[_currentMenu]->set_focusSymbol(position, symbol);
}


template <class Disp>
bool LiquidSystem<Disp>::set_focusGlyph(Position position, uint8_t glyph) {
    return _p_liquidMenu[_currentMenu]->set_focusSymbol(position, glyph);
}

template <class Disp>
bool LiquidSystem<Disp>::call_function(uint8_t number) const {
	bool returnValue = _p_liquidMenu[_currentMenu]->call_function(number);
	_p_liquidMenu[_currentMenu]->_p_liquidCrystal->clear();
	update();
	return returnValue;
}

template <class Disp>
void LiquidSystem<Disp>::update() const {
	_p_liquidMenu[_currentMenu]->update();
}

template <class Disp>
void LiquidSystem<Disp>::softUpdate() const {
	_p_liquidMenu[_currentMenu]->softUpdate();
}

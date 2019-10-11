/**
@file
Contains the LiquidMenu class definition.
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
#include "glyphs.h"
#include <type_traits>
#include<Arduino.h>

#pragma once

const uint8_t DIVISION_LINE_LENGTH = 40; ///< Sets the length of the division line.

//SFINAE Test for DrawTile

template <typename T>
class HasCreateChar
{
private:
    typedef char YesType[1];
    typedef char NoType[2];
    template <typename C> static YesType& test( decltype(static_cast<void(C::*)(uint8_t,  uint8_t *)>(&C::createChar)) ) ; //&C::createChar
    template <typename C> static NoType& test(...);
    
    
public:
    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
    
};

template<typename T>
typename std::enable_if<HasCreateChar<T>::value, void>::type
CallCreateChar(T *t, uint8_t c, uint8_t *tile_ptr) {
    /* something when T has drawTile ... */
    t->createChar(c, tile_ptr);
}
void CallCreateChar(...);

template <class Disp>
LiquidMenu<Disp>::LiquidMenu(Disp &liquidCrystal, uint8_t startingScreen)
  : _p_liquidCrystal(&liquidCrystal), _screenCount(0),
    _currentScreen(startingScreen - 1) {
#ifndef I2C
        init();
#endif
//
// _p_liquidCrystal->createChar(static_cast<uint8_t>(FocusIndicator::RIGHT), glyph::rightFocus);
//  _p_liquidCrystal->createChar(static_cast<uint8_t>(FocusIndicator::LEFT), glyph::leftFocus);
//  _p_liquidCrystal->createChar(static_cast<uint8_t>(FocusIndicator::CUSTOM), glyph::customFocus);
//#endif
}

template <class Disp>
LiquidMenu<Disp>::LiquidMenu(Disp &liquidCrystal, LiquidScreen<Disp> &liquidScreen,
                       uint8_t startingScreen)
  : LiquidMenu(liquidCrystal, startingScreen) {
      add_screen(liquidScreen);
}

template <class Disp>
LiquidMenu<Disp>::LiquidMenu(Disp &liquidCrystal, LiquidScreen<Disp> &liquidScreen1,
                       LiquidScreen<Disp> &liquidScreen2, uint8_t startingScreen)
  : LiquidMenu(liquidCrystal, liquidScreen1, startingScreen) {
      add_screen(liquidScreen2);
}

template <class Disp>
LiquidMenu<Disp>::LiquidMenu(Disp &liquidCrystal, LiquidScreen<Disp> &liquidScreen1,
                       LiquidScreen<Disp> &liquidScreen2, LiquidScreen<Disp> &liquidScreen3,
                       uint8_t startingScreen)
  : LiquidMenu(liquidCrystal, liquidScreen1, liquidScreen2, startingScreen) {
  add_screen(liquidScreen3);
}

template <class Disp>
LiquidMenu<Disp>::LiquidMenu(Disp &liquidCrystal, LiquidScreen<Disp> &liquidScreen1,
                       LiquidScreen<Disp> &liquidScreen2, LiquidScreen<Disp> &liquidScreen3,
                       LiquidScreen<Disp> &liquidScreen4, uint8_t startingScreen)
  : LiquidMenu(liquidCrystal, liquidScreen1, liquidScreen2, liquidScreen3,
               startingScreen) {
  add_screen(liquidScreen4);
}

template <class Disp>
bool LiquidMenu<Disp>::add_screen(LiquidScreen<Disp> &liquidScreen) {
  print_me(reinterpret_cast<uintptr_t>(this));
  if (_screenCount < MAX_SCREENS) {
    _p_liquidScreen[_screenCount] = &liquidScreen;
    DEBUG(F("Added a new screen (")); DEBUG(_screenCount); DEBUGLN(F(")"));
    _screenCount++;
    return true;
  }
  DEBUG(F("Adding screen ")); DEBUG(_screenCount);
  DEBUGLN(F(" failed, edit LiquidMenu_config.h to allow for more screens"));
  return false;
}

template <class Disp>
LiquidScreen<Disp>* LiquidMenu<Disp>::get_currentScreen() const {
  return _p_liquidScreen[_currentScreen];
}

template <class Disp>
void LiquidMenu<Disp>::next_screen() {
  _p_liquidCrystal->clear();
  do {
    if (_currentScreen < _screenCount - 1)  {
      _currentScreen++;
    } else {
      _currentScreen = 0;
    }
  } while (_p_liquidScreen[_currentScreen]->_hidden == true);
  update();
  DEBUG(F("Switched to the next screen (")); DEBUG(_currentScreen); DEBUG(F(")"));
}

template <class Disp>
void LiquidMenu<Disp>::operator++() {
  next_screen();
}

template <class Disp>
void LiquidMenu<Disp>::operator++(int) {
  next_screen();
}

template <class Disp>
void LiquidMenu<Disp>::previous_screen() {
  _p_liquidCrystal->clear();
  do {
    if (_currentScreen > 0) {
      _currentScreen--;
    } else {
      _currentScreen = _screenCount - 1;
    }
  } while (_p_liquidScreen[_currentScreen]->_hidden == true);
  update();
  DEBUG(F("Switched to the previous screen (")); DEBUG(_currentScreen); DEBUGLN(F(")"));
}

template <class Disp>
void LiquidMenu<Disp>::operator--() {
  previous_screen();
}

template <class Disp>
void LiquidMenu<Disp>::operator--(int) {
  previous_screen();
}

template <class Disp>
bool LiquidMenu<Disp>::change_screen(uint8_t number) {
  uint8_t index = number - 1;
  if (index <= _screenCount) {
    _p_liquidCrystal->clear();
    _currentScreen = index;
    update();
    DEBUG(F("Switched to screen ("));
    DEBUG(_currentScreen); DEBUGLN(F(")"));
    return true;
  } else {
    DEBUG(F("Invalid request for screen change to ")); DEBUGLN(number);
    return false;
  }
}

template <class Disp>
bool LiquidMenu<Disp>::change_screen(LiquidScreen<Disp> &p_liquidScreen) {
  // _p_liquidMenu[_currentMenu]->_p_liquidCrystal->clear();
  for (uint8_t s = 0; s < _screenCount; s++) {
    _p_liquidCrystal->clear();
    if (reinterpret_cast<uintptr_t>(&p_liquidScreen) == reinterpret_cast<uintptr_t>(&(*_p_liquidScreen[s]))) {
      _currentScreen = s;
      update();
      DEBUG(F("Switched to screen ("));
      DEBUG(_currentScreen); DEBUGLN(F(")"));
      update();
      return true;
    }
  }
  DEBUG(F("Invalid request for screen change to 0x")); DEBUGLN(reinterpret_cast<uintptr_t>(&p_liquidScreen));
  return false;
}

template <class Disp>
bool LiquidMenu<Disp>::operator=(uint8_t number) {
  return change_screen(number);
}

template <class Disp>
bool LiquidMenu<Disp>::operator=(LiquidScreen<Disp> &p_liquidScreen) {
  return change_screen(p_liquidScreen);
}

template <class Disp>
void LiquidMenu<Disp>::switch_focus(bool forward) {
  _p_liquidCrystal->clear();
  _p_liquidScreen[_currentScreen]->switch_focus(forward);
  update();
}

template <class Disp>
bool LiquidMenu<Disp>::set_focusPosition(Position position) {
  print_me(reinterpret_cast<uintptr_t>(this));
  if (position == Position::CUSTOM) {
    DEBUGLN(F("Can't set a custom focus position for the whole menu at once"));
    return false;
  } else {
    DEBUG(F("Focus position set to ")); DEBUGLN((uint8_t)position);
    for (uint8_t s = 0; s < _screenCount; s++) {
      _p_liquidScreen[s]->set_focusPosition(position);
    }
    return true;
  }
}

template <class Disp>
bool LiquidMenu<Disp>::set_focusSymbol(Position position, uint8_t symbol[GLYPH_SIZE]) {
  switch (position) {
  case Position::RIGHT: {
    //_p_liquidCrystal->createChar(FocusIndicator::RIGHT, symbol);
      for (int x = 0; x < GLYPH_SIZE; x++) {
          _customFocusGlyphs[static_cast<uint8_t>(position)][x] = symbol[x];
      }
    DEBUG(F("Right"));
    break;
  } //case RIGHT
  case Position::LEFT: {
    //_p_liquidCrystal->createChar(FocusIndicator::LEFT, symbol);
      for (int x = 0; x < GLYPH_SIZE; x++) {
          _customFocusGlyphs[static_cast<uint8_t>(position)][x] = symbol[x];
      }
      DEBUG(F("Left"));
    break;
  } //case LEFT
  case Position::CUSTOM: {
    //_p_liquidCrystal->createChar(FocusIndicator::CUSTOM, symbol);
      for (int x = 0; x < GLYPH_SIZE; x++) {
          _customFocusGlyphs[static_cast<uint8_t>(position)][x] = symbol[x];
      }
    DEBUG(F("Custom"));
    break;
  } //case CUSTOM
  default: {
    DEBUGLN(F("Invalid focus position, options are 'RIGHT', 'LEFT' and 'CUSTOM'"));
    return false;
  } //default
  } //switch (position)
  DEBUGLN(F("Focus symbol changed to:"));
  for (uint8_t i = 0; i < 8; i++) {
    DEBUGLN2(symbol[i], BIN);
  }
  return true;
}
template <class Disp>
bool LiquidMenu<Disp>::set_focusGlyph(Position position, uint8_t glyph) {
    switch (position) {
        case Position::RIGHT: {
            //_p_liquidCrystal->createChar(FocusIndicator::RIGHT, symbol);
            _focusGlyphs[static_cast<uint8_t>(position)] = glyph;
            DEBUG(F("Right"));
            break;
        } //case RIGHT
        case Position::LEFT: {
            //_p_liquidCrystal->createChar(FocusIndicator::LEFT, symbol);
            _focusGlyphs[static_cast<uint8_t>(position)] = glyph;
            DEBUG(F("Left"));
            break;
        } //case LEFT
        case Position::CUSTOM: {
            //_p_liquidCrystal->createChar(FocusIndicator::CUSTOM, symbol);
            _focusGlyphs[static_cast<uint8_t>(position)] = glyph;
            DEBUG(F("Custom"));
            break;
        } //case CUSTOM
        default: {
            DEBUGLN(F("Invalid focus position, options are 'RIGHT', 'LEFT' and 'CUSTOM'"));
            return false;
        } //default
    } //switch (position)
    DEBUGLN(F("Focus glyph changed to: "));
    DEBUGLN2(glyph, HEX);
    return true;
}

template <class Disp>
bool LiquidMenu<Disp>::call_function(uint8_t number) const {
  bool returnValue = _p_liquidScreen[_currentScreen]->call_function(number);
  update();
  return returnValue;
}

template <class Disp>
void LiquidMenu<Disp>::update() const {
  _p_liquidCrystal->clear();
  softUpdate();
}

template <class Disp>
void LiquidMenu<Disp>::softUpdate() const {
  DEBUGLN(F("Updating the LCD"));
  for (uint8_t b = 0; b < DIVISION_LINE_LENGTH; b++) {
    DEBUG(F("-"));
  }
  DEBUGLN();
  DEBUG(F("|Screen ")); DEBUGLN(_currentScreen);
  _p_liquidScreen[_currentScreen]->print(_p_liquidCrystal, _focusGlyphs, _customFocusGlyphs);
  for (uint8_t b = 0; b < DIVISION_LINE_LENGTH; b++) {
    DEBUG(F("-"));
  }
  DEBUGLN("\n");
}

template <class Disp>
void LiquidMenu<Disp>::init() {
  //_p_liquidCrystal->createChar(static_cast<uint8_t>(FocusIndicator::RIGHT), glyph::rightFocus);
  //_p_liquidCrystal->createChar(static_cast<uint8_t>(FocusIndicator::LEFT), glyph::leftFocus);
  //_p_liquidCrystal->createChar(static_cast<uint8_t>(FocusIndicator::CUSTOM), glyph::customFocus);
    Serial.print("HasCreateChar ");
    Serial.println(HasCreateChar<Disp>::value);
    if (HasCreateChar<Disp>::value) {
        CallCreateChar(_p_liquidCrystal, static_cast<uint8_t>(FocusIndicator::RIGHT), glyph::rightFocus);
        CallCreateChar(_p_liquidCrystal, static_cast<uint8_t>(FocusIndicator::LEFT), glyph::leftFocus);
        CallCreateChar(_p_liquidCrystal, static_cast<uint8_t>(FocusIndicator::CUSTOM), glyph::customFocus);
    }
    for(int x = 0; x < GLYPH_SIZE; x++) {
        _customFocusGlyphs[static_cast<uint8_t>(Position::RIGHT)][x] = glyph::rightFocus[x];
        _customFocusGlyphs[static_cast<uint8_t>(Position::LEFT)][x] = glyph::leftFocus[x];
        _customFocusGlyphs[static_cast<uint8_t>(Position::CUSTOM)][x] = glyph::customFocus[x];
    }
    _focusGlyphs[static_cast<uint8_t>(Position::RIGHT)] = static_cast<uint8_t>(FocusIndicator::RIGHT);
    _focusGlyphs[static_cast<uint8_t>(Position::LEFT)] = static_cast<uint8_t>(FocusIndicator::LEFT);
    _focusGlyphs[static_cast<uint8_t>(Position::CUSTOM)] = static_cast<uint8_t>(FocusIndicator::CUSTOM);
}

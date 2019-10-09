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
Contains the LiquidLine class definition.
*/

#include "LiquidMenu.h"
#include <Arduino.h>
#pragma once

//SFINAE test for createChar
template <typename T>
class HasCreateChar
{
private:
    typedef char YesType[1];
    typedef char NoType[2];
    
    template <typename C> static YesType& test( decltype(&C::createChar) ) ;
    template <typename C> static NoType& test(...);
    
    
public:
    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

template<typename T>
typename std::enable_if<HasCreateChar<T>::value, void>::type
CallCreateChar(T * t, uint8_t c, uint8_t g[]) {
    /* something when T has toString ... */
    t->createChar( c, g);
    return;
}

template <typename T>
class HasDrawTile
{
private:
    typedef char YesType[1];
    typedef char NoType[2];
    
    template <typename C> static YesType& test( decltype(&C::drawTile) ) ;
    template <typename C> static NoType& test(...);
    
    
public:
    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};
void CallCreateChar(...);

template<typename T>
typename std::enable_if<HasDrawTile<T>::value, void>::type
CallDrawTile(T *t, uint8_t x, uint8_t y, int cnt, const uint8_t *tile_ptr) {
    /* something when T has drawTile ... */
    t->drawTile( x,  y,  cnt, tile_ptr);
    return;
}
void CallDrawTile(...);


template<typename T>
typename std::enable_if<HasDrawTile<T>::value, uint8_t>::type
CallGetX(T *t) {
    /* something when T has drawTile ... */
    return t->tx;
}
uint8_t CallGetX(...);


template<typename T>
typename std::enable_if<HasDrawTile<T>::value, uint8_t>::type
CallGetY(T *t) {
    /* something when T has drawTile ... */
    return t->ty;
}
uint8_t CallGetY(...);

template <class Disp>
bool LiquidLine<Disp>::attach_function(uint8_t number, void (*function)(void)) {
	print_me(reinterpret_cast<uintptr_t>(this));
	if (number <= MAX_FUNCTIONS) {
		_function[number - 1] = function;
		DEBUG(F("Attached function ")); DEBUGLN(number);
		_focusable = true;
		return true;
	} else {
		DEBUG(F("Attaching function ")); DEBUG(number);
		DEBUGLN(F(" failed, edit LiquidMenu_config.h to allow for more functions"));
		return false;
	}
}

template <class Disp>
void LiquidLine<Disp>::set_decimalPlaces(uint8_t decimalPlaces)
{
	_floatDecimalPlaces = decimalPlaces;
}
template <class Disp>
bool LiquidLine<Disp>::set_focusPosition(Position position, uint8_t column, uint8_t row) {
	print_me(reinterpret_cast<uintptr_t>(this));
	if (position <= Position::CUSTOM) {
		_focusPosition = position;
		_focusColumn = column;
		_focusRow = row;
		DEBUG(F("Focus position set to ")); DEBUG((uint8_t)_focusPosition);
		if (_focusPosition == Position::CUSTOM) {
			DEBUG(F(" at (")); DEBUG(_focusRow); DEBUG(F(", "));
			DEBUG(_focusColumn); DEBUG(F(")"));
		}
		DEBUGLN();
		return true;
	} else {
		DEBUGLN(F("Failed setting focus position, options are 'RIGHT', 'LEFT' and 'CUSTOM'"));
		return false;
	}
}
template <class Disp>
bool LiquidLine<Disp>::set_asGlyph(uint8_t number) {
	uint8_t index = number - 1;
	if ( (index < MAX_VARIABLES) && (_variableType[index] == DataType::UINT8_T) ) {
		_variableType[index] = DataType::GLYPH;
		return true;
	} else {
		DEBUG(F("Setting variable ")); DEBUG(number);
		DEBUGLN(F(" as glyph failed, the variable must be of 'byte' data type"));
		return false;
	}
}
template <class Disp>
bool LiquidLine<Disp>::set_asProgmem(uint8_t number) {
	uint8_t index = number - 1;
	if ((index < MAX_VARIABLES) && (_variableType[index] == DataType::CONST_CHAR_PTR)) {
		_variableType[index] = DataType::PROG_CONST_CHAR_PTR;
		return true;
	}
	else {
		DEBUG(F("Setting variable ")); DEBUG(number);
		DEBUGLN(F(" as PROG_CONST_CHAR failed, the variable must be of 'const char[]' data type"))
		return false;
	}
}
template <class Disp>
void LiquidLine<Disp>::print(Disp *p_liquidCrystal, const uint8_t focusGlyphs[], const uint8_t customFocusGlyphs[][GLYPH_SIZE], bool isFocused) {
	p_liquidCrystal->setCursor(_column, _row);
	DEBUG(F(" (")); DEBUG(_column); DEBUG(F(", ")); DEBUG(_row); DEBUGLN(F(")"));

	DEBUG(F("|\t"));
	for (uint8_t v = 0; v < MAX_VARIABLES; v++) {
		print_variable(p_liquidCrystal, v);
	}
    
	DEBUGLN();
    //Serial.print(F("CreateChar ")); Serial.println(HasCreateChar<Disp>::value);
    //Serial.print(F("DrawTile ")); Serial.println(HasDrawTile<Disp>::value);
	if (isFocused) {
		DEBUG(F("\t\t<Focus position: "));
		switch (_focusPosition) {
		case Position::RIGHT: {
            uint8_t c = focusGlyphs[static_cast<uint8_t>(Position::RIGHT) ];
            const uint8_t * g = customFocusGlyphs[static_cast<uint8_t>(Position::RIGHT)];
            if (c == static_cast<uint8_t>(FocusIndicator::RIGHT)) {
                if(HasCreateChar<Disp>::value) {
                    CallCreateChar(p_liquidCrystal, c, g);
                    p_liquidCrystal->write(c);

                } else if (HasDrawTile<Disp>::value) {
                    uint8_t * rot = rotTile (g);

                    CallDrawTile(p_liquidCrystal, CallGetX(p_liquidCrystal), CallGetY(p_liquidCrystal), 1, rot);
                    //p_liquidCrystal->drawTile(p_liquidCrystal->tx, p_liquidCrystal->ty, 1, g);
                    delete rot;

                } else {
                    p_liquidCrystal->write(c);
                }
            } else {
                p_liquidCrystal->write(c);
            }
			DEBUGLN(F("right>"));
			//p_liquidCrystal->print(NOTHING);
			break;
		} //case RIGHT
		case Position::LEFT: {
			//p_liquidCrystal->print(NOTHING);
			p_liquidCrystal->setCursor(_column - 1, _row);
            uint8_t c = focusGlyphs[static_cast<uint8_t>(Position::LEFT) ];
            const uint8_t * g = customFocusGlyphs[static_cast<uint8_t>(Position::LEFT)];
            if (c == static_cast<uint8_t>(FocusIndicator::LEFT)) {
                if(HasCreateChar<Disp>::value) {
                    CallCreateChar(p_liquidCrystal, c, g);
                    p_liquidCrystal->write(c);
                    
                } else if (HasDrawTile<Disp>::value) {
                    uint8_t * rot = rotTile (g);
                    CallDrawTile(p_liquidCrystal, _column -1, _row, 1, rot);
                    //p_liquidCrystal->drawTile(_column -1, _row, 1, g);
                    delete rot;
                } else {
                    p_liquidCrystal->write(c);
                }
            } else {
                p_liquidCrystal->write(c);
            }
			DEBUGLN(F("left>"));
			break;
		} //case LEFT
        case Position::LEFTRIGHT: {
            uint8_t c = focusGlyphs[static_cast<uint8_t>(Position::RIGHT) ];
            const uint8_t * g = customFocusGlyphs[static_cast<uint8_t>(Position::RIGHT)];
            if (c == static_cast<uint8_t>(FocusIndicator::RIGHT)) {
                if(HasCreateChar<Disp>::value) {
                    CallCreateChar(p_liquidCrystal, c, g);
                    p_liquidCrystal->write(c);
                    
                } else if (HasDrawTile<Disp>::value) {
                    uint8_t * rot = rotTile (g);
                     CallDrawTile(p_liquidCrystal, CallGetX(p_liquidCrystal), CallGetY(p_liquidCrystal), 1, rot);
                    //p_liquidCrystal->drawTile(p_liquidCrystal->tx, p_liquidCrystal->ty, 1, g);
                    delete rot;
                    
                } else {
                    p_liquidCrystal->write(c);
                }
            } else {
                p_liquidCrystal->write(c);
            }
            //p_liquidCrystal->print(NOTHING);
            p_liquidCrystal->setCursor(_column - 1, _row);
            c = focusGlyphs[static_cast<uint8_t>(Position::LEFT) ];
            g = customFocusGlyphs[static_cast<uint8_t>(Position::LEFT)];
            if (c == static_cast<uint8_t>(FocusIndicator::LEFT)) {
                if(HasCreateChar<Disp>::value) {
                    CallCreateChar(p_liquidCrystal, c, g);
                    p_liquidCrystal->write(c);
                    
                } else if (HasDrawTile<Disp>::value) {
                    uint8_t * rot = rotTile (g);
                    CallDrawTile(p_liquidCrystal, _column -1, _row, 1, rot);
                    //p_liquidCrystal->drawTile(_column -1, _row, 1, g);
                    delete rot;
                } else {
                    p_liquidCrystal->write(c);
                }
            } else {
                p_liquidCrystal->write(c);
            }
            DEBUGLN(F("leftright>"));
            break;
        } //case LEFTRIGHT
		case Position::CUSTOM: {
			//p_liquidCrystal->print(NOTHING);
			p_liquidCrystal->setCursor(_focusColumn, _focusRow);
            uint8_t c = focusGlyphs[static_cast<uint8_t>(Position::CUSTOM) ];
            const uint8_t * g = customFocusGlyphs[static_cast<uint8_t>(Position::CUSTOM)];

            if (c == static_cast<uint8_t>(FocusIndicator::CUSTOM)) {
                if(HasCreateChar<Disp>::value) {
                    CallCreateChar(p_liquidCrystal, c, g);
                    p_liquidCrystal->write(c);
                    
                } else if (HasDrawTile<Disp>::value) {
                    uint8_t * rot = rotTile (g);
                    CallDrawTile(p_liquidCrystal, _focusColumn, _focusRow, 1, rot);
                    //p_liquidCrystal->drawTile( _focusColumn, _focusRow, 1, g);
                    delete rot;
                } else {
                    p_liquidCrystal->write(c);
                }
            } else {
                p_liquidCrystal->write(c);
            }
            DEBUGLN(F("custom (")); DEBUG(_focusColumn);
			DEBUG(F(", ")); DEBUG(_focusRow); DEBUGLN(F(")>"));
			break;
		} //case CUSTOM
		default: {
			DEBUG(F("invalid (")); DEBUG((uint8_t)_focusPosition);
			DEBUGLN(F("), switching to default>"));
            uint8_t c = focusGlyphs[static_cast<uint8_t>(Position::RIGHT) ];
            const uint8_t * g = customFocusGlyphs[static_cast<uint8_t>(Position::RIGHT)];
			_focusPosition = Position::NORMAL;
            if (c == static_cast<uint8_t>(FocusIndicator::RIGHT)) {
                if(HasCreateChar<Disp>::value) {
                    CallCreateChar(p_liquidCrystal, c, g);
                    p_liquidCrystal->write(c);
                    
                } else if (HasDrawTile<Disp>::value) {
                    uint8_t * rot = rotTile(g);
                     CallDrawTile(p_liquidCrystal, CallGetX(p_liquidCrystal), CallGetY(p_liquidCrystal), 1, rot);
                    delete rot;
                } else {
                    p_liquidCrystal->write(c);
                }
            } else {
                p_liquidCrystal->write(c);
            }
            //p_liquidCrystal->print(NOTHING);
			break;
		} //default
		} //switch (_focusPosition)
	} else {
		//p_liquidCrystal->print(NOTHING);
	}
}

template <class Disp>
void LiquidLine<Disp>::print_variable(Disp *p_liquidCrystal, uint8_t number) {
	switch (_variableType[number]) {

    // Variables -----
	case DataType::CONST_CHAR_PTR: {
		const char* variable = reinterpret_cast<const char*>(_variable[number]);
		DEBUG(F("(const char*)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case CONST_CHAR_PTR
	case DataType::CHAR_PTR: {
		char* variable = *reinterpret_cast<char**>( const_cast<void*>(_variable[number]) );
		// char* variable = const_cast<char*>(reinterpret_cast<const char *>(_variable[number]));
		DEBUG(F("(char*)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case CHAR_PTR
	case DataType::CHAR: {
		const char variable = *static_cast<const char*>(_variable[number]);
		DEBUG(F("(char)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case CHAR

	case DataType::INT8_T: {
		const int8_t variable = *static_cast<const int8_t*>(_variable[number]);
		DEBUG(F("(int8_t)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case INT8_T
	case DataType::UINT8_T: {
		const uint8_t variable = *static_cast<const uint8_t*>(_variable[number]);
		DEBUG(F("(uint8_t)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case UINT8_T

	case DataType::INT16_T: {
		const int16_t variable = *static_cast<const int16_t*>(_variable[number]);
		DEBUG(F("(int16_t)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case INT16_T
	case DataType::UINT16_T: {
		const uint16_t variable = *static_cast<const uint16_t*>(_variable[number]);
		DEBUG(F("(uint16_t)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case UINT16_T

	case DataType::INT32_T: {
		const int32_t variable = *static_cast<const int32_t*>(_variable[number]);
		DEBUG(F("(int32_t)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case INT32_T
	case DataType::UINT32_T: {
		const uint32_t variable = *static_cast<const uint32_t*>(_variable[number]);
		DEBUG(F("(uint32_t)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case UINT32_T

	case DataType::FLOAT: {
		const float variable = *static_cast<const float*>(_variable[number]);
		DEBUG(F("(float)")); DEBUG(variable);
		p_liquidCrystal->print(variable, _floatDecimalPlaces);
		break;
	} //case FLOAT

	case DataType::BOOL: {
		const bool variable = *static_cast<const bool*>(_variable[number]);
		DEBUG(F("(bool)")); DEBUG(variable);
		p_liquidCrystal->print(variable);
		break;
	} //case BOOL

	case DataType::GLYPH: {
		const uint8_t variable = *static_cast<const uint8_t*>(_variable[number]);
		DEBUG(F("(glyph)")); DEBUG(variable);
		p_liquidCrystal->write((uint8_t)variable);
		break;
	} //case BOOL

	case DataType::PROG_CONST_CHAR_PTR: {
		const char* variable = reinterpret_cast<const char*>(_variable[number]);
		volatile const int len = strlen_P(variable);
		char buffer[len];
		for (uint8_t i = 0; i < len; i++) {
			buffer[i] = pgm_read_byte_near(variable + i);
		}
		buffer[len] = '\0';
		DEBUG(F("(const char*)")); DEBUG(buffer);
		p_liquidCrystal->print(buffer);
		break;
	} //case PROG_CONST_CHAR_PTR
    // ~Variables -----

    // Getter functions -----
	case DataType::CONST_CHAR_PTR_GETTER: {
		const constcharPtrFnPtr getterFunction = reinterpret_cast<constcharPtrFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			const char * variable = (getterFunction)();
		    DEBUG(F("(const char*)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case CONST_CHAR_PTR_GETTER

	case DataType::CHAR_PTR_GETTER: {
		const charPtrFnPtr getterFunction = reinterpret_cast<charPtrFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			char* variable = (getterFunction)();
			DEBUG(F("(char*)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case CHAR_PTR_GETTER

	case DataType::CHAR_GETTER: {
		const charFnPtr getterFunction = reinterpret_cast<charFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			char variable = (getterFunction)();
			DEBUG(F("(char)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case CHAR_GETTER

	case DataType::INT8_T_GETTER: {
		const int8tFnPtr getterFunction = reinterpret_cast<int8tFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			int8_t variable = (getterFunction)();
			DEBUG(F("(int8_t)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case INT8_T_GETTER

	case DataType::UINT8_T_GETTER: {
		const uint8tFnPtr getterFunction = reinterpret_cast<uint8tFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			uint8_t variable = (getterFunction)();
			DEBUG(F("(uint8_t)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case UINT8_T_GETTER

	case DataType::INT16_T_GETTER: {
		const int16tFnPtr getterFunction = reinterpret_cast<int16tFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			int16_t variable = (getterFunction)();
			DEBUG(F("(int16_t)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case INT16_T_GETTER

	case DataType::UINT16_T_GETTER: {
		const uint16tFnPtr getterFunction = reinterpret_cast<uint16tFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			uint16_t variable = (getterFunction)();
			DEBUG(F("(uint16_t)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case UINT16_T_GETTER

	case DataType::INT32_T_GETTER: {
		const int32tFnPtr getterFunction = reinterpret_cast<int32tFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			int32_t variable = (getterFunction)();
			DEBUG(F("(int32_t)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case INT32_T_GETTER

	case DataType::UINT32_T_GETTER: {
		const uint32tFnPtr getterFunction = reinterpret_cast<uint32tFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			uint32_t variable = (getterFunction)();
			DEBUG(F("(uint32_t)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case UINT32_T_GETTER

	case DataType::FLOAT_GETTER: {
		const floatFnPtr getterFunction = reinterpret_cast<floatFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			const float variable = (getterFunction)();
			DEBUG(F("(float)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case FLOAT_GETTER

	case DataType::BOOL_GETTER: {
		const boolFnPtr getterFunction = reinterpret_cast<boolFnPtr>(_variable[number]);
		if (getterFunction != nullptr) {
			bool variable = (getterFunction)();
			DEBUG(F("(bool)")); DEBUG(variable);
			p_liquidCrystal->print(variable);
		} 
		break;
	} // case BOOL_GETTER
    // ~Getter functions -----

	default: { break; }

	} //switch (_variableType)
	DEBUG(F(" "));
}

template <class Disp>
bool LiquidLine<Disp>::call_function(uint8_t number) const {
	if (_function[number - 1]) {
		(*_function[number - 1])();
		return true;
	} else {
		return false;
	}
}

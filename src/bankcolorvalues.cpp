/*
 *  Project:    moba-ambilight
 *
 *  Version:    1.0.0
 *
 *  Copyright (C) 2016 Stefan Paproth <pappi-@gmx.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/agpl.txt>.
 *
 */

#include "BankColorValues.h"

BankColorValues::BankColorValues(int white = 0, int green = 0, int red = 0, int blue = 0) {
    for(int b = 0; b < BANK_COUNT; ++b) {
        value[b][WHITE] = white;
        value[b][GREEN] = green;
        value[b][RED]   = red;
        value[b][BLUE]  = blue;
    }
}

BankColorValues::BankColorValues(const BankColorValues &orig) {
    setAll(orig);
}

BankColorValues::~BankColorValues() {
}

void BankColorValues::setColor(BankColor color, int val) {
    for(int b = 0; b < BANK_COUNT; ++b) {
        setColor(b, color, val);
    }
}

void BankColorValues::setColor(int bank, BankColor color, int val) {
    if(val < Bridge::MIN_VALUE) {
        val = Bridge::MIN_VALUE;
    }
    if(val > Bridge::MAX_VALUE) {
        val = Bridge::MAX_VALUE;
    }
    value[bank][color] = val;
}

void BankColorValues::setAll(const BankColorValues &val) {
    for(int c = 0; c < COLOR_COUNT; ++c) {
        for(int b = 0; b < BANK_COUNT; ++b) {
            value[b][c] = val.value[b][c];
        }
    }
}

int BankColorValues::getColor(int bank, BankColor color) {
    return value[bank][color];
}

void BankColorValues::increment(int bank, BankColor color) {
    setColor(bank, color, value[bank][color]++);
}

void BankColorValues::decrement(int bank, BankColor color) {
    setColor(bank, color, value[bank][color]--);
}

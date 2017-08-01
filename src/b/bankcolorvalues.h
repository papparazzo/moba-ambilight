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

#pragma once

class BankColorValues {
    public:
        enum BankColor {
            WHITE       = 0,
            GREEN       = 1,
            RED         = 2,
            BLUE        = 3,
            COLOR_COUNT = 4
        };

        static const int BANK_COUNT = 4;

        BankColorValues(int white = 0, int green = 0, int red = 0, int blue = 0);
        BankColorValues(const BankColorValues &orig);

        ~BankColorValues();

        void setColor(BankColor color, int val);
        void setColor(int bank, BankColor color, int val);
        void setAll(const BankColorValues &val);
        unsigned int getColor(int bank, BankColor color);
        void increment(int bank, BankColor color);
        void decrement(int bank, BankColor color);

    protected:
        int value[BANK_COUNT][COLOR_COUNT];
};
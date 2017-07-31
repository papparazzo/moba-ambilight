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

#include <string>
#include <exception>

#include <boost/noncopyable.hpp>

#include "bankcolorvalues.h"

class BridgeException : public std::exception {

    public:
        explicit BridgeException(const std::string &err) throw() : what_(err) {
        }

        BridgeException() throw() : what_("Unknown error") {
        }

        virtual ~BridgeException() throw() {
        }

        virtual const char *what() const throw() {
            return what_.c_str();
        }

    private:
        std::string what_;
};

class Bridge : private boost::noncopyable {

    public:
        static const int MAX_VALUE = 4095;
        static const int MIN_VALUE = 0;
        static const int BANK_COUNT = 4;

        Bridge(int address = 0x40);
        ~Bridge();

        void softwareReset();
        void setPWMFrequency(int freq);

        void setOn(BankColorValues::BankColor color);
        void setOff(BankColorValues::BankColor color);
        void setPWM(BankColorValues::BankColor color, int on, int off);
        void setPWMlg(BankColorValues::BankColor color, int val);
        void setPWMlg(BankColorValues::BankColor color, int bank, int val);
        void setPWMlg(const BankColorValues &values, int bank);
        void setPWMlg(const BankColorValues &values);
        void setData(BankColorValues::BankColor color, int bank, int on, int off);
        void setAllOff();

    protected:
        // Registers/etc.
        static const int MODE1         = 0x00;
        static const int MODE2         = 0x01;
        static const int SUBADR1       = 0x02;
        static const int SUBADR2       = 0x03;
        static const int SUBADR3       = 0x04;
        static const int PRESCALE      = 0xFE;
        static const int LED0_ON_L     = 0x06;
        static const int LED0_ON_H     = 0x07;
        static const int LED0_OFF_L    = 0x08;
        static const int LED0_OFF_H    = 0x09;
        static const int ALL_LED_ON_L  = 0xFA;
        static const int ALL_LED_ON_H  = 0xFB;
        static const int ALL_LED_OFF_L = 0xFC;
        static const int ALL_LED_OFF_H = 0xFD;

        // Bits
        static const int RESTART       = 0x80;
        static const int SLEEP         = 0x10;
        static const int ALLCALL       = 0x01;
        static const int INVRT         = 0x10;
        static const int OUTDRV        = 0x04;

        int fd;

    private:
};

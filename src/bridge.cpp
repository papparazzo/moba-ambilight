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

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <unistd.h>
#include <moba/helper.h>

#include "bridge.h"

namespace {
    const int table[] = {
        0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,
        6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12,
        12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21,
        22, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
        38, 39, 40, 42, 43, 44, 46, 47, 49, 50, 52, 54, 55, 57, 59, 61, 63, 65,
        67, 69, 71, 73, 75, 78, 80, 83, 86, 88, 91, 94, 97, 100, 103, 106, 110,
        113, 117, 121, 124, 128, 132, 137, 141, 145, 150, 155, 160, 165, 170,
        175, 181, 187, 192, 199, 205, 211, 218, 225, 232, 239, 247, 255, 263,
        271, 280, 289, 298, 307, 317, 327, 338, 348, 359, 371, 382, 394, 407,
        420, 433, 447, 461, 476, 491, 506, 522, 539, 556, 574, 592, 611, 630,
        650, 671, 692, 714, 736, 760, 784, 809, 834, 861, 888, 916, 945, 975,
        1006, 1038, 1071, 1105, 1140, 1176, 1213, 1251, 1291, 1332, 1374, 1418,
        1463, 1509, 1557, 1606, 1657, 1710, 1764, 1820, 1878, 1937, 1998, 2062,
        2127, 2195, 2264, 2336, 2410, 2486, 2565, 2646, 2730, 2817, 2906, 2998,
        3093, 3191, 3292, 3397, 3504, 3615, 3730, 3848, 3970, 4095
    };
}

Bridge::Bridge(int address) {
    this->fd = wiringPiI2CSetup(address);
    if(this->fd == -1) {
        throw BridgeException(moba::getErrno("wiringPiI2CSetup"));
    }
    this->setAllOff();

    wiringPiI2CWriteReg8(this->fd, Bridge::MODE2, Bridge::OUTDRV);
    wiringPiI2CWriteReg8(this->fd, Bridge::MODE1, Bridge::ALLCALL);
    usleep(5000);
    int mode1 = wiringPiI2CReadReg8(this->fd, Bridge::MODE1);
    mode1 = mode1 & ~Bridge::SLEEP;
    wiringPiI2CWriteReg8(this->fd, Bridge::MODE1, mode1);
    usleep(5000);
}

Bridge::~Bridge() {
    close(this->fd);
}

void Bridge::softwareReset() {
    int fd = wiringPiI2CSetup(0x00);
    if(fd == -1) {
        throw BridgeException(moba::getErrno("wiringPiI2CSetup"));
    }

    // Sends a software reset (SWRST) command to all the drivers on the bus
    if(wiringPiI2CWrite(fd, 0x06) == -1) {
        throw BridgeException(moba::getErrno("wiringPiI2CWrite"));
    }

    if(close(fd) == -1) {
        throw BridgeException(moba::getErrno("close"));
    }
}

void Bridge::setPWMFrequency(int freq) {
    float oscClock = 25000000.0; // 25MHz
    oscClock /= 4096.0;          // 12-bit
    oscClock /= (float)freq;
    oscClock -= 1.0;

    int prescale = (int)oscClock + 0.5;

    int oldmode = wiringPiI2CReadReg8(this->fd, Bridge::MODE1);
    int newmode = (oldmode & 0x7F) | 0x10;
    wiringPiI2CWriteReg8(this->fd, Bridge::MODE1, newmode);
    wiringPiI2CWriteReg8(this->fd, Bridge::PRESCALE, prescale);
    wiringPiI2CWriteReg8(this->fd, Bridge::MODE1, oldmode);
    usleep(5000);
    wiringPiI2CWriteReg8(this->fd, Bridge::MODE1, oldmode | 0x80);
}

void Bridge::setOn(BankColor color) {
    for(int i = 0; i < 4; ++i) {
        this->setData(color, i, 4096, 0);
    }
}

void Bridge::setOff(BankColor color) {
    for(int i = 0; i < 4; ++i) {
        this->setData(color, i, 0, 0);
    }
}

void Bridge::setPWM(BankColor color, int on, int off) {
    for(int i = 0; i < 4; ++i) {
        this->setData(color, i, on, off);
    }
}

void Bridge::setPWMlg(BankColor color, unsigned char val) {
    for(int i = 0; i < 4; ++i) {
        this->setData(color, i, 0, table[val]);
    }
}

void Bridge::setData(Bridge::BankColor color, int bank, int on, int off) {
    int channel = (bank * 4 + color) * 4;

    wiringPiI2CWriteReg8(this->fd, Bridge::LED0_ON_L + channel, on & 0xFF);
    wiringPiI2CWriteReg8(this->fd, Bridge::LED0_ON_H + channel, on >> 8);
    wiringPiI2CWriteReg8(this->fd, Bridge::LED0_OFF_L + channel, off & 0xFF);
    wiringPiI2CWriteReg8(this->fd, Bridge::LED0_OFF_H + channel, off >> 8);
}

void Bridge::setAllOff() {
    wiringPiI2CWriteReg8(this->fd, Bridge::ALL_LED_ON_L, 0);
    wiringPiI2CWriteReg8(this->fd, Bridge::ALL_LED_ON_H, 0);
    wiringPiI2CWriteReg8(this->fd, Bridge::ALL_LED_OFF_L, 0);
    wiringPiI2CWriteReg8(this->fd, Bridge::ALL_LED_OFF_H, 0);
}

const char *Bridge::getColorName(BankColor c) {
    switch(c) {
        case RED:
            return "red";

        case BLUE:
            return "blue";

        case WHITE:
            return "white";

        case GREEN:
            return "green";
    }
}

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
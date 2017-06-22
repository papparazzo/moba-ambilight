#include "plasma.h"
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

#include <cmath>
#include <iostream>
#include <wiringPi.h>

#define PI 3.1415926535897932384626433832795

Plasma::Plasma(boost::shared_ptr<Bridge> b) {
    bridge  = b;

    setAmlitudeAndOffset(Bridge::WHITE,   0, 2000);
    setAmlitudeAndOffset(Bridge::GREEN, 500, 2000);
    setAmlitudeAndOffset(Bridge::RED,   500,  800);
    setAmlitudeAndOffset(Bridge::BLUE,  500, 1000);
}

void Plasma::setAmlitudeAndOffset(Bridge::BankColor color, int amplitude, int offset) {
    if(amplitude * 2 + offset > Bridge::MAX_VALUE) {
        throw PlasmaException("amplitude and offset to high!");
    }

    if(offset < Bridge::MIN_VALUE) {
        throw PlasmaException("amplitude and offset to low!");
    }

    range[color].amplitude =  amplitude;
    range[color].offset = offset;
}

double Plasma::d(int x, double t) {
    double v = std::sin(t + 10.0 * x);

    double r = 1 + std::sin(v * PI);
    double g = 1 + std::sin(v * PI + 2 * PI / 3);
    double b = 1 + std::sin(v * PI + 4 * PI / 3);

    // 0 - 2
    bridge->setPWMlg(Bridge::WHITE, x,  2000);
    bridge->setPWMlg(Bridge::GREEN, x, (500 * g + 2000));
    bridge->setPWMlg(Bridge::RED, x, (500 * r + 800));
    bridge->setPWMlg(Bridge::BLUE, x, (500 * b + 1000));
}

void Plasma::next() {
//    delayMicroseconds(750);
    counter = ++counter % 200000;
    d(0, (double)counter * PI / 1000);  //rechts
    d(1, (double)counter * PI / 1000);  //mitte
    d(2, (double)counter * PI / 1000);  //links
}

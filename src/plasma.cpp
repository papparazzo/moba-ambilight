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

Plasma::Plasma(
    boost::shared_ptr<Bridge> bridge,
    boost::shared_ptr<moba::SignalHandler> sigTerm
) {
    bridge    = bridge;
    sigTerm   = sigTerm;
}

double Plasma::d(int x, double t) {
    return std::sin(t + 10.0 * x);
}

double Plasma::d1(int x, double t) {
    return std::sin(10.0 * (x * std::sin(t / 2) + std::cos(t / 3)) + t);
}

double Plasma::d2(int x, double t) {
    double cx = x + 0.5 * std::sin(t / 5);
    double cy = 1 + 0.5 * std::cos(t / 3);
    return std::sin(std::sqrt(100.0 * (std::pow(cx, 2) + std::pow(cy, 2)) + 1) + t);
}

double Plasma::d3(int x, double t) {
    double v = d(x, t) + d1(x, t) + d2(x, t);

    double r = 1 + std::sin(v * PI);
    double g = 1 + std::sin(v *  PI + 2 * PI / 3);
    double b = 1 + std::sin(v * PI + 4 * PI / 3);

    std::cerr << "x: " << x << " t: " << t << " ";
    std::cerr << "red: " << (4094 * r) << " green: " << (4094 * g) << " blue: " << (4094 * b) << std::endl;
    bridge->setPWMlg(Bridge::RED, x, (2000 * r));
    bridge->setPWMlg(Bridge::GREEN, x, (2000 * g));
    bridge->setPWMlg(Bridge::BLUE, x, (2000 * b));

}

void Plasma::run() {
    int i = 0;
    do {
        if(sigTerm->hasAnySignalTriggered()) {
            return;
        }
        delayMicroseconds(50);
        i = ++i % 1000;
        d3(0, (double)i * PI / 500);
        d3(1, (double)i * PI / 500);
        d3(2, (double)i * PI / 500);
    } while(true);
}
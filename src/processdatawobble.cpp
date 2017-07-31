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

#include "processdatawobble.h"

namespace {
    const double PI = 3.1415926535897932384626433832795;
}

bool ProcessDataWobble::next(bool setOutput) {
    counter = ++counter % 200000;
    d(0, (double)counter * PI / 1000);  // right
    d(1, (double)counter * PI / 1000);  // center
    d(2, (double)counter * PI / 1000);  // left
}

void ProcessDataWobble::setAmlitudeAndOffset(Bridge::BankColor color, int amplitude, int offset) {
    if(amplitude * 2 + offset > Bridge::MAX_VALUE) {
        throw ProcessDataException("amplitude and offset to high!");
    }

    if(offset < Bridge::MIN_VALUE) {
        throw ProcessDataException("amplitude and offset to low!");
    }

    range[color].amplitude =  amplitude;
    range[color].offset = offset;
}

double ProcessDataWobble::d(int bank, double t) {
    bridge->setPWMlg(Bridge::GREEN, bank, getPlasmaValue(Bridge::GREEN, bank, t));
    bridge->setPWMlg(Bridge::RED,   bank, getPlasmaValue(Bridge::RED,   bank, t));
    bridge->setPWMlg(Bridge::BLUE,  bank, getPlasmaValue(Bridge::BLUE,  bank, t));
    bridge->setPWMlg(Bridge::WHITE, bank, getPlasmaValue(Bridge::WHITE, bank, t));
}


double Controller::getPlasmaValue(Bridge::BankColor color, int bank, double t) {
    double v = std::sin(t + 10.0 * bank);

    switch(color) {
        case Bridge::BLUE:
            v = 1 + std::sin(v * PI + 4 * PI / 3);
            break;

        case Bridge::RED:
            v = 1 + std::sin(v * PI);
            break;

        case Bridge::GREEN:
            v = 1 + std::sin(v * PI + 2 * PI / 3);
            break;

        case Bridge::WHITE:
            v = 1 + std::sin(v * PI + 8 * PI / 3);
            break;
    }

    return range[color].amplitude * v + range[color].offset;
}


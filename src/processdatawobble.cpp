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

#include <cmath>

namespace {
    const double PI = 3.1415926535897932384626433832795;
}

ProcessDataWobble::ProcessDataWobble(boost::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, const BankColorValues &amp, unsigned int dur) :
ProcessData(bridge, start, end, dur), amplitude(amp) {
    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        for(int c = 0; c < 4; ++c) {
            unsigned int offset = end.getValue(b, c);
            if(amp.getValue(b, c) * 2 + offset > Bridge::MAX_VALUE) {
                throw ProcessDataException("amplitude and offset to high!");
            }
            if(offset < Bridge::MIN_VALUE) {
                throw ProcessDataException("amplitude and offset to low!");
            }
        }
    }
}

bool ProcessDataWobble::next(bool setOutput) {
    if(!ProcessData::next(setOutput)) {
        return false;
    }
    counter = ++counter % 200000;
    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        for(int c = 0; c < 4; ++c) {
            current.setValue(b, c, getWobbleValue(b, c, counter));
        }
        if(setOutput) {
            bridge->setPWMlg(current, b);
        }
    }
    return false;
}

double ProcessDataWobble::getWobbleValue(int b, int c, unsigned int i) {
    double v = std::sin((double)i * PI / 1000 + 10.0 * b);
    switch(c) {
        case BankColorValues::BLUE:
            v = 1 + std::sin(v * PI + 4 * PI / 3);
            break;

        case BankColorValues::RED:
            v = 1 + std::sin(v * PI);
            break;

        case BankColorValues::GREEN:
            v = 1 + std::sin(v * PI + 2 * PI / 3);
            break;

        case BankColorValues::WHITE:
            v = 1 + std::sin(v * PI + 8 * PI / 3);
            break;
    }
    return amplitude.getValue(b, c) * v + target.getValue(b, c);
}

unsigned int ProcessDataWobble::getBankColors(BankColorValues &values, unsigned int stepsAhead) {
    if(ProcessData::getBankColors(values, stepsAhead) == 0) {
        return 0;
    }



    return 0;
}
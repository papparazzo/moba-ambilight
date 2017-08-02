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

#include "processdata.h"

unsigned int ProcessData::objCounter = 0;

ProcessData::ProcessData(boost::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, unsigned int dur) :
bridge(bridge), current(start), target(end), duration(dur) {
    objNumber = objCounter++;
    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        for(int c = 0; c < BankColorValues::COLOR_COUNT; ++c) {
            int delta = end.getValue(b, c) - start.getValue(b, c);
            if(delta) {
                stepWidth.setValue(b, c, TOTAL_STEPS_COUNT / delta);
            } else {
                stepWidth.setValue(b, c, 0);
            }
        }
    }
}

ProcessData::~ProcessData() {

}

unsigned int ProcessData::getObjectId() const {
    return objNumber;
}

unsigned int ProcessData::getDuration() const {
    return duration;
}

bool ProcessDataPlain::next(bool setOutput) {
    if(++counter == TOTAL_STEPS_COUNT) {
        return true;
    }
    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        for(int c = 0; c < 4; ++c) {
            unsigned int cc = stepWidth.getValue(b, c);
            if(!cc || counter % cc) {
                continue;
            }
            if(cc > 0 && current.getValue(b, c) < RANGE) {
                current.increment(b, c);
            }
            if(cc < 0 && current.getValue(b, c) > 0) {
                current.decrement(b, c);
            }
        }
        if(setOutput) {
            bridge->setPWMlg(current, b);
        }
    }
    return false;
}

/*
BankColorValues ProcessData::getBankColors(int stepsAhead) {
    //stepsAhead * stepWidth counter
}

*/
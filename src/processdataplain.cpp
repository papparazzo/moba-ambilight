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

#include "processdataplain.h"

ProcessDataPlain::ProcessDataPlain(boost::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, unsigned int dur = 0) :
ProcessData(bridge, start, end, dur) {

}

bool ProcessDataPlain::next(bool setOutput) {
    return ProcessData::next(setOutput);
}


/*
BankColorValues ProcessData::getBankColors(int stepsAhead, int bank) {
    if(stepsAhead > TOTAL_STEPS_COUNT) {
        throw ProcessDataException("out of range");
    }

    for(int c = 0; c < BankColorValues::COLOR_COUNT; ++c) {
        if(!stepWidth.getColor(bank, c) || i % stepWidth.getColor(bank, c)) {
            continue;
        }
        if(stepWidth.getColor(bank, c) > 0 && current.getColor(bank, c) < Controller::RANGE) {
            current.increment(bank, c);
        }
        if(stepWidth.getColor(bank, c) < 0 && current.getColor(bank, c) > 0) {
            current.decrement(bank, c);
        }
    }
    //stepsAhead * stepWidth counter
}

*/
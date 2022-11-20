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
#include <moba-common/log.h>

unsigned int ProcessData::objCounter = 0;

ProcessData::ProcessData(std::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, unsigned int dur) :
bridge(bridge), current(start), target(end), counter(0) {
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

    LOG(moba::common::LogLevel::DEBUG) << "--> item #" << objNumber << std::endl;
    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        LOG(moba::common::LogLevel::DEBUG) <<
            "target of bank ["  << b << "] " <<
            " white: " << target.getValue(b, BankColorValues::WHITE) <<
            " green: " << target.getValue(b, BankColorValues::GREEN) <<
            " red: " << target.getValue(b, BankColorValues::RED) <<
            " blue: " << target.getValue(b, BankColorValues::BLUE) << std::endl;
    }
    setInterruptionTime(dur);
}

ProcessData::~ProcessData() {

}

unsigned int ProcessData::getObjectId() const {
    return objNumber;
}

unsigned int ProcessData::getInterruptionTime() const {
    return interruptionTime;
}

bool ProcessData::hasNext(bool setOutput) {
    if(++counter == TOTAL_STEPS_COUNT) {
        return false;
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
            if(setOutput) {
                bridge->setPWMlg(c, b, current.getValue(b, c));
            }
        }
    }
    return true;
}

unsigned int ProcessData::getBankColors(BankColorValues &values, unsigned int stepsAhead) const {
    if(counter + stepsAhead > TOTAL_STEPS_COUNT) {
        return counter + stepsAhead - TOTAL_STEPS_COUNT;
    }

    unsigned int delta = TOTAL_STEPS_COUNT - (counter + stepsAhead);
    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        for(int c = 0; c < 4; ++c) {
            unsigned int sw = stepWidth.getValue(b, c);
            values.setValue(b, c, current.getValue(b, c) + delta / sw);
        }
    }
    return 0;
}

void ProcessData::setInterruptionTime(unsigned int duration) {
    LOG(moba::common::LogLevel::DEBUG) <<
        "duration total: ~" << duration <<
        " sec. (~" << (int)(duration / 60) << " min.)" << std::endl;

    interruptionTime = static_cast<int>((duration * 1000 * 1000) / ProcessData::TOTAL_STEPS_COUNT);
    LOG(moba::common::LogLevel::DEBUG) << "interruption: ~" << interruptionTime << " µs." << std::endl;
}

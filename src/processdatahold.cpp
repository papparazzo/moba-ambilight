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

#include "processdatahold.h"
#include <moba-common/log.h>

ProcessDataHold::ProcessDataHold(std::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, unsigned int dur = 0) :
ProcessData(bridge, start, end, dur) {
    LOG(moba::common::LogLevel::DEBUG) << "type: [hold] " << std::endl;
}

bool ProcessDataHold::hasNext(bool setOutput) {
    ProcessData::hasNext(setOutput);
    return true;
}

unsigned int ProcessDataHold::getBankColors(BankColorValues &values, unsigned int stepsAhead) const {
    if(ProcessData::getBankColors(values, stepsAhead) != 0) {
        values = target;
    }
    return 0;
}

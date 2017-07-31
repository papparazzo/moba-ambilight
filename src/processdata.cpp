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

unsigned int ProcessData::counter = 0;

ProcessData::~ProcessData() {

}

unsigned int ProcessData::getObjectId() const {
    return objNb;
}

unsigned int ProcessData::getDuration() const {
    return duration;
}





ProcessData::ProcessData(const BankColorValues &start, const BankColorValues &end, unsigned int dur) :
current(start), target(end), duration(dur)
{
    objNb = counter++;

    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        for(int c = 0; c < BankColorValues::COLOR_COUNT; ++c) {
            int delta = end.getColor(b, c) - start.getColor(b, c);
            if(delta) {
                stepWidth.setColor(b, c, TOTAL_STEPS_COUNT / delta);
            } else {
                stepWidth.setColor(b, c, 0);
            }
        }
    }
}


/**

    for(int b = 0; b < 3; ++b) {
        for(int c = 0; c < 4; ++c) {
            if(!stepWidth[c] || i % stepWidth[c]) {
                return;
            }
            if(stepWidth[c] > 0 && current.targetIntensity[c] < Controller::RANGE) {
                current.targetIntensity[c]++;
            }
            if(stepWidth[c] < 0 && current.targetIntensity[c] > 0) {
                current.targetIntensity[c]--;
            }
            if(interrupted) {
                bridge->setPWMlg(Controller::bcolor[c], b, current.targetIntensity[c]);
            }
        }
    }

 *






        } else if(newValues.wobble) {
            //current.wobble = true;
            //setAmlitudeAndOffset((Bridge::BankColor)i, newValues.targetIntensity[i], current.targetIntensity[i]);

            stepWidth[i] = Controller::STEPS / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            stepWidth[i] = 0;
 */


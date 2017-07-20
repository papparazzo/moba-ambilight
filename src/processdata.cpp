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

ProcessData::ProcessData() {
    targetIntensity[0] = 0;
    targetIntensity[1] = 0;
    targetIntensity[2] = 0;
    targetIntensity[3] = 0;
    wobble = false;
    direkt = false;
    objNb = counter++;
}

ProcessData::~ProcessData() {

}

unsigned int ProcessData::getObjectId() const {
    return objNb;
}

Bridge::BankColorValues ProcessData::getBankColors(int stepsAhead, int bank) {
    if(stepsAhead > TOTAL_STEPS_COUNT) {
        throw ProcessDataException("out of range");
    }

    if(wobble) {

    }
}


/**
 Bridge::BankColorValues Controller::getBankColors(int stepsAhead, int bank) {
    if(counter + stepsAhead > TOTAL_STEPS_COUNT) {
        // return getFuture(counter + stepsAhead - TOTAL_STEPS_COUNT);
    }

    if(!stepWidth[c] || i % stepWidth[c]) {
        return;
    }
    if(stepWidth[c] > 0 && current.targetIntensity[c] < Controller::RANGE) {
        current.targetIntensity[c]++;
    }
    if(stepWidth[c] < 0 && current.targetIntensity[c] > 0) {
        current.targetIntensity[c]--;
    }

    //stepsAhead * stepWidth counter
}

 * /

void Controller::setNextTarget(const TargetValues &newValues) {
    //current.wobble = false;
    for(int i = 0; i < 4; ++i) {
        /*
        if(!duration || newValues.direkt) {
            //current.targetIntensity[i] = newValues.targetIntensity[i];
            //bridge->setPWMlg(Controller::bcolor[i], current.targetIntensity[i]);
        } else if(newValues.wobble) {
            //current.wobble = true;
            //setAmlitudeAndOffset((Bridge::BankColor)i, newValues.targetIntensity[i], current.targetIntensity[i]);
/*
            stepWidth[i] = Controller::STEPS / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            stepWidth[i] = 0;
 */

/*

        } else if(newValues.targetIntensity[i] - current.targetIntensity[i]) {
            stepWidth[i] = TOTAL_STEPS_COUNT / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            stepWidth[i] = 0;
        }
    }
}
 * */
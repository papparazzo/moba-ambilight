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
        if(!duration) {
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


void Controller::setAmlitudeAndOffset(Bridge::BankColor color, int amplitude, int offset) {
    if(amplitude * 2 + offset > Bridge::MAX_VALUE) {
        throw ControllerException("amplitude and offset to high!");
    }

    if(offset < Bridge::MIN_VALUE) {
        throw ControllerException("amplitude and offset to low!");
    }

    range[color].amplitude =  amplitude;
    range[color].offset = offset;
}

double Controller::d(int bank, double t) {
    bridge->setPWMlg(Bridge::GREEN, bank, getPlasmaValue(Bridge::GREEN, bank, t));
    bridge->setPWMlg(Bridge::RED,   bank, getPlasmaValue(Bridge::RED,   bank, t));
    bridge->setPWMlg(Bridge::BLUE,  bank, getPlasmaValue(Bridge::BLUE,  bank, t));
    bridge->setPWMlg(Bridge::WHITE, bank, getPlasmaValue(Bridge::WHITE, bank, t));
}

void Controller::plasmanext() {
    counter = ++counter % 200000;
    d(0, (double)counter * PI / 1000);  // right
    d(1, (double)counter * PI / 1000);  // center
    d(2, (double)counter * PI / 1000);  // left
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

void Controller::stepRegular() {

    if(halted) {
        return;
    }


/*
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
*/

    i = i++ % TOTAL_STEPS_COUNT;

}


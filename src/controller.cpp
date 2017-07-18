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

#include "controller.h"

namespace {
    const double PI = 3.1415926535897932384626433832795;
    const Bridge::BankColor Controller::bcolor[] = {
        Bridge::BLUE,
        Bridge::GREEN,
        Bridge::RED,
        Bridge::WHITE
    };
}

Controller::Controller(boost::shared_ptr<Bridge> b) {
    bridge  = b;
    duration = DEFAULT_DURATION;
}

Controller::~Controller() {
}

bool Controller::next() {


    TargetValues regular;
    TargetValues interrupt;

    int i;
    do {
        ++i;
        delayMicroseconds(duration + current.duration);
        if(i % regular.duration) {

        }
        if(i % interrupt.duration) {

        }
    } while(true);




    if(!regularBuffer.hasItems()) {
        return false;
    }

    TargetValues target = regularBuffer.pop();



current;

/*

    do {
        delayMicroseconds(duration + current.duration);
        plasma->next();
    } while(true);


    do {
        fetchNextMsg();
        delayMicroseconds(duration + current.duration);
        blas();


    } while(true);
 **/
}


void Controller::setNextTarget(const TargetValues &newValues) {
    current.wobble = false;
    for(int i = 0; i < 4; ++i) {
        if(!duration || newValues.direkt) {
            current.targetIntensity[i] = newValues.targetIntensity[i];
            bridge->setPWMlg(Controller::bcolor[i], current.targetIntensity[i]);
        } else if(newValues.wobble) {
            current.wobble = true;
            setAmlitudeAndOffset(i, newValues.targetIntensity[i], current.targetIntensity[i]);
/*
            stepWidth[i] = Controller::STEPS / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            stepWidth[i] = 0;
 */


        } else if(newValues.targetIntensity[i] - current.targetIntensity[i]) {
            stepWidth[i] = Controller::STEPS / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            stepWidth[i] = 0;
        }
    }
}

void Controller::resume() {
    interrupted = false;
    interruptBuffer.reset();
}

void Controller::emergencyStop(int brigthness, int duration) {

}

void Controller::releaseEmergencyStop() {

}


TargetValues Controller::prefetch(int step) {

}

void Controller::runTestMode() {
    bridge->setPWMlg(Bridge::BLUE, 0);
    bridge->setPWMlg(Bridge::GREEN, 0);
    bridge->setPWMlg(Bridge::WHITE, 0);
    bridge->setPWMlg(Bridge::RED, 211);
    sleep(1);
    bridge->setPWMlg(Bridge::RED, 0);
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 2; ++k) {
                bridge->setData(Controller::bcolor[j], i, 0, 150);
                delay(500);
                bridge->setData(Controller::bcolor[j], i, 0, 600);
                delay(500);
            }
            bridge->setData(Controller::bcolor[j], i, 0, 0);
        }
    }
    bridge->setPWMlg(Bridge::GREEN, 211);
    sleep(1);
    bridge->setAllOff();
    sleep(1);
}

void Controller::reset() {
    regularBuffer.reset();
    interruptBuffer.reset();
    bridge->setAllOff();
}

void Controller::setNewTarget(const TargetValues& newValues, bool immediately) {
    if(!immediately) {
        regularBuffer.push(newValues);
        return;
    }
    if(interrupted) {
        interruptBuffer.push(newValues);
        return;
    }
    interrupted = true;
    // FIXME: Set new values
}

void Controller::setDuration(int d) {
    duration = static_cast<int>((d * 1000 * 1000) / Controller::STEPS);
}

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




void stepRegular() {
    if(doCount) {
        return;
    }

    if(!output) {
        return;
    }
}

void stepInterupt() {

}


blas() {
    for(int j = 0; j < 4; ++j) {
        if(!step[j] || i % step[j]) {
            return;
        }
        if(step[j] > 0 && current.targetIntensity[j] < Handler::RANGE) {
            current.targetIntensity[j]++;
        }
        if(step[j] < 0 && current.targetIntensity[j] > 0) {
            current.targetIntensity[j]--;
        }
        bridge->setPWMlg(Handler::bcolor[j], current.targetIntensity[j]);
    }

    i = i++ % Handler::STEPS;

    if(i) {
        continue;
    }
}


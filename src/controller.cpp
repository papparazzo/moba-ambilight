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
}

Controller::Controller(boost::shared_ptr<Bridge> b) {
    bridge  = b;
/*
    setAmlitudeAndOffset(Bridge::WHITE,   0, 2000);
    setAmlitudeAndOffset(Bridge::GREEN, 500, 2000);
    setAmlitudeAndOffset(Bridge::RED,   500,  800);
    setAmlitudeAndOffset(Bridge::BLUE,  500, 1000);
 */
}

Controller::~Controller() {
}

void Controller::setNextTarget(const TargetValues &newValues) {
/*

    for(int i = 0; i < 4; ++i) {
        if(!duration || newValues.direkt) {
            current.targetIntensity[i] = newValues.targetIntensity[i];
            bridge->setPWMlg(Handler::bcolor[i], current.targetIntensity[i]);
        } else if(newValues.targetIntensity[i] - current.targetIntensity[i]) {
            step[i] = Handler::STEPS / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            step[i] = 0;
        }
    }
 */
}


bool Controller::next() {



/*


    TargetValues target;

 */
// FIXME: Hold for x seconds???

    if(!regularBuffer.hasItems()) {
        return false;
    }

    TargetValues target = regularBuffer.pop();

/*

    if(target.direkt) {
        continue;
    }

    if(target.wobble) {
        Plasma plasma(bridge);
        plasma.setAmlitudeAndOffset(Bridge::WHITE, 0, 2000);
        plasma.setAmlitudeAndOffset(Bridge::GREEN, 500, 2000);
        plasma.setAmlitudeAndOffset(Bridge::RED, 500, 800);
        plasma.setAmlitudeAndOffset(Bridge::BLUE, 500, 1000);

        do {
            if(sigTerm->hasAnySignalTriggered()) {
                return;
            }
            delayMicroseconds(duration + current.duration);
            plasma->next();
        } while(true);

    }

    do {
        fetchNextMsg();
        delayMicroseconds(duration + current.duration);
        for(int j = 0; j < 4; ++j) {
            if(!step[j] || i % step[j]) {
                continue;
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



    } while(true);
 **/
}


void Controller::reset() {
    regularBuffer.reset();
//                interruptBuffer.reset();

}


void Controller::setDuration(int d) {
    duration = static_cast<int>((d * 1000 * 1000) / Controller::STEPS);
}

void Plasma::setAmlitudeAndOffset(Bridge::BankColor color, int amplitude, int offset) {
    if(amplitude * 2 + offset > Bridge::MAX_VALUE) {
        throw ControllerException("amplitude and offset to high!");
    }

    if(offset < Bridge::MIN_VALUE) {
        throw ControllerException("amplitude and offset to low!");
    }

    range[color].amplitude =  amplitude;
    range[color].offset = offset;
}

double Plasma::d(int x, double t) {
    double v = std::sin(t + 10.0 * x);

    double r = 1 + std::sin(v * PI);              // 0 - 2
    double g = 1 + std::sin(v * PI + 2 * PI / 3); // 0 - 2
    double b = 1 + std::sin(v * PI + 4 * PI / 3); // 0 - 2
    double w = 1 + std::sin(v * PI + 8 * PI / 3);

    bridge->setPWMlg(Bridge::GREEN, x, (range[Bridge::GREEN].amplitude * g + range[Bridge::GREEN].offset));
    bridge->setPWMlg(Bridge::RED,   x, (range[Bridge::RED  ].amplitude * r + range[Bridge::RED  ].offset));
    bridge->setPWMlg(Bridge::BLUE,  x, (range[Bridge::BLUE ].amplitude * b + range[Bridge::BLUE ].offset));
    bridge->setPWMlg(Bridge::WHITE, x, (range[Bridge::WHITE].amplitude * w + range[Bridge::WHITE].offset));
}

void Plasma::next() {
    counter = ++counter % 200000;
    d(0, (double)counter * PI / 1000);  // right
    d(1, (double)counter * PI / 1000);  // center
    d(2, (double)counter * PI / 1000);  // left
}

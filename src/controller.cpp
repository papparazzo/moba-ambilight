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

#include "controller.h"

Controller::Controller() {
}

Controller::~Controller() {
}

void Controller::setTargetValues(const TargetValues &newValues) {
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


bool Controller::nextStep() {
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
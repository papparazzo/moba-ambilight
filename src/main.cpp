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

#include <wiringPi.h>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cerrno>

#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>

#include <moba/atomic.h>
#include <moba/ipc.h>
#include <moba/helper.h>
#include <moba/log.h>

const int PIN_OUT_R = 27;
const int PIN_OUT_B = 24;
const int PIN_OUT_W = 23;
const int PIN[3] = {PIN_OUT_R, PIN_OUT_B, PIN_OUT_W};

const int FACTOR = 5;
const int RANGE  = 1000;

moba::Atomic<bool> running(true);
moba::Atomic<int> currRatio[3];

void *triggerR_(void *) {
    while(running) {
        if(currRatio[0] == RANGE || currRatio[0] == 0) {
            delay(RANGE);
            continue;
        }

        delayMicroseconds((RANGE - currRatio[0]) * FACTOR);
        digitalWrite(PIN[0], HIGH);
        delayMicroseconds(currRatio[0] * FACTOR);
        digitalWrite(PIN[0], LOW);
    }
}

void *triggerB_(void *) {
    while(running) {
        if(currRatio[1] == RANGE || currRatio[1] == 0) {
            delay(RANGE);
            continue;
        }

        delayMicroseconds((RANGE - currRatio[1]) * FACTOR);
        digitalWrite(PIN[1], HIGH);
        delayMicroseconds(currRatio[1] * FACTOR);
        digitalWrite(PIN[1], LOW);
    }
}

void *triggerW_(void *) {
    while(running) {
        if(currRatio[2] == RANGE || currRatio[2] == 0) {
            delay(RANGE);
            continue;
        }

        delayMicroseconds((RANGE - currRatio[2]) * FACTOR);
        digitalWrite(PIN[2], HIGH);
        delayMicroseconds(currRatio[2] * FACTOR);
        digitalWrite(PIN[2], LOW);
    }
}

void loop() {
    int target[4];
    int step[3];

    moba::IPC ipc(moba::IPC::READING, "/tmp/fifo0001");
    std::string data;
    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    while(running) {
        ipc.readLine(data);
        for(int i = 0; i < 4; ++i) {
            found = data.find(';', pos);
            target[i] = atoi(data.substr(pos, found - pos).c_str());
            pos = found + 1;
        }
        for(int i = 0; i < 3; ++i) {
            if(!(target[i] - currRatio[i])) {
                step[i] = RANGE;
            } else {
                step[i] = RANGE / (target[i] - currRatio[i]);
            }
        }
        for(int i = 0; i < RANGE; ++i) {
            delay(target[3]);
            for(int j = 0; j < 3; ++j) {
                if(!(i % step[j])) {
                    if(step[j] > 0 && step[j] < RANGE) {
                        currRatio[j]++;
                    } else if(step[j] < RANGE) {
                        currRatio[j]--;
                    }
                }
            }
        }
        delayMicroseconds(RANGE * FACTOR * 2);
        for(int i = 0; i < 3; ++i) {
            if(target[i] == 0) {
                currRatio[i] = 0;
                digitalWrite(PIN[i], LOW);
            } else if(target[i] == RANGE) {
                currRatio[i] = RANGE;
                digitalWrite(PIN[i], HIGH);
            }
        }
    }
}

int main(int argc, char** argv) {
    setpriority(PRIO_PROCESS, 0, -20);

    struct sched_param param;
    param.sched_priority = 79;
    if(sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
        LOG(moba::ERROR) << moba::getErrno("sched_setscheduler failed") << std::endl;
        exit(EXIT_FAILURE);
    }

    wiringPiSetup();

    for(int i = 0; i < 3; ++i) {
        pinMode(PIN[i], OUTPUT);
    }

    pthread_t thR;
    pthread_create(&thR, NULL, triggerR_, NULL);
    pthread_detach(thR);

    pthread_t thB;
    pthread_create(&thB, NULL, triggerB_, NULL);
    pthread_detach(thB);

    pthread_t thW;
    pthread_create(&thW, NULL, triggerW_, NULL);
    pthread_detach(thW);

    try {
        loop();
    } catch(std::exception &e) {
        LOG(moba::WARNING) << e.what() << std::endl;
    }

    for(int i = 0; i < 3; ++i) {
        digitalWrite(PIN[i], LOW);
    }

}

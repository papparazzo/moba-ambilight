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

const int DEF_FACTOR = 5;
const int DEF_RANGE  = 1000;
const int DEF_STEPS  = DEF_RANGE * 10;

int FACTOR = DEF_FACTOR;
int RANGE  = DEF_RANGE;
int STEPS  = DEF_STEPS;

moba::Atomic<bool> running(true);
moba::Atomic<int> currRatio[3];

void *triggerR_(void *) {
    while(running) {
        if(currRatio[0] == RANGE || currRatio[0] == 0) {
            delayMicroseconds(RANGE * FACTOR);
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
            delayMicroseconds(RANGE * FACTOR);
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
            delayMicroseconds(RANGE * FACTOR);
            continue;
        }

        delayMicroseconds((RANGE - currRatio[2]) * FACTOR);
        digitalWrite(PIN[2], HIGH);
        delayMicroseconds(currRatio[2] * FACTOR);
        digitalWrite(PIN[2], LOW);
    }
}

void test() {
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            delay(500);
            digitalWrite(PIN[i], HIGH);
            delay(500);
            digitalWrite(PIN[i], LOW);
        }
    }
}

void loop(const std::string &fifo) {
    int target[4];
    int step[3];

    moba::IPC ipc(moba::IPC::READING, fifo);
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
            if(target[i] - currRatio[i]) {
                step[i] = STEPS / (target[i] - currRatio[i]);
            } else {
                step[i] = STEPS + 1;
            }
        }
        for(int i = 1; i <= STEPS; ++i) {
            delay(target[3]);
            for(int j = 0; j < 3; ++j) {
                if(i % step[j]) {
                    continue;
                }
                if(step[j] > 0 && currRatio[j] == RANGE) {
                    continue;
                }
                if(step[j] > 0 ) {
                    currRatio[j]++;
                }
                if(step[j] > 0 && currRatio[j] == RANGE) {
                    digitalWrite(PIN[j], HIGH);
                    continue;
                }
                if(step[j] < 0 && currRatio[j] == 0) {
                    continue;
                }
                if(step[j] < 0) {
                    currRatio[j]--;
                }
                if(step[j] < 0 && currRatio[j] == 0) {
                    digitalWrite(PIN[j], LOW);
                    continue;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    std::string fifo = "/tmp/fifo0001";

    switch(argc) {
        case 4:
            fifo = std::string(argv[3]);

        case 3:
            RANGE = atoi(argv[2]);

        case 2:
            FACTOR = atoi(argv[1]);

        default:
            break;
    }

    struct sched_param param;
    param.sched_priority = 79;
    if(sched_setscheduler(0, SCHED_RR, &param) != 0) {
        LOG(moba::ERROR) << moba::getErrno("sched_setscheduler failed") << std::endl;
        exit(EXIT_FAILURE);
    }

    wiringPiSetup();

    for(int i = 0; i < 3; ++i) {
        pinMode(PIN[i], OUTPUT);
    }

//test();
//return 0;
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
        loop(fifo);
    } catch(std::exception &e) {
        LOG(moba::WARNING) << e.what() << std::endl;
    }

    for(int i = 0; i < 3; ++i) {
        digitalWrite(PIN[i], LOW);
    }
}
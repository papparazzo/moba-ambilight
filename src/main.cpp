/*
 * File:   main.cpp
 * Author: stefan
 *
 * Created on August 5, 2016, 10:40 PM
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

const int PIN_OUT_R = 27;
const int PIN_OUT_B = 24;
const int PIN_OUT_W = 23;
const int PIN[3] = {PIN_OUT_R, PIN_OUT_B, PIN_OUT_W};

const int FACTOR = 50;
const int RANGE  = 255;

Atomic<bool> running(true);
Atomic<int> currRatio[3];

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

    IPC ipc(IPC::READING, "/tmp/fifo0001");
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
            step[i] = RANGE / (target[i] - currRatio[i]);
        }
        for(int i = 0; i < RANGE; ++i) {
            delay(target[3]);
            for(int j = 0; j < 3; ++j) {
                if(!(i % step[j])) {
                    if(step[j] > 0) {
                        currRatio[j]++;
                    } else {
                        currRatio[j]--;
                    }
                }
            }
        }
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
        std::cout << errno << std::endl;
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

    while(true) {
        try {
            loop();
        } catch(...) {
            // --#--
        }
    }
}

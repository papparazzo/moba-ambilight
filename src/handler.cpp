/*
 *  Project:    CommonLib
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

#include "handler.h"
#include <moba/log.h>
#include <wiringPi.h>
#include <string.h>

const Bridge::BankColor Handler::bcolor[] = {
    Bridge::BLUE,
    Bridge::GREEN,
    Bridge::RED,
    Bridge::WHITE
};

Handler::Handler(
    boost::shared_ptr<Bridge> bridge,
    boost::shared_ptr<moba::IPC> ipc,
    boost::shared_ptr<moba::SignalHandler> sigTerm
) {
    this->bridge = bridge;
    this->ipc = ipc;
    this->sigTerm = sigTerm;
    for(int i = 0; i < 4; ++i) {
        this->currRatio[i] = 0;
    }
}

void Handler::emergencyOn() {
    LOG(moba::DEBUG) << "emergencyOn" << std::endl;
    this->bridge->setPWM(Bridge::BLUE,  0, 0);
    this->bridge->setPWM(Bridge::GREEN, 0, 0);
    this->bridge->setPWM(Bridge::RED,   0, 0);
    this->bridge->setPWM(Bridge::WHITE, 0, 4095);
}

void Handler::emergencyOff() {
    LOG(moba::DEBUG) << "emergencyOff" << std::endl;
    for(int j = 0; j < 4; ++j) {
        this->bridge->setPWM(Handler::bcolor[j], 0, this->currRatio[j]);
    }
}

void Handler::emergency() {
    this->emergencyOn();
    while(!this->ipc->receive(moba::IPC::EMERGENCY_RELEASE)) {
        usleep(50000);
    }
    this->emergencyOff();
}

void Handler::test() {
    LOG(moba::DEBUG) << "testing... " << std::endl;
    this->bridge->setPWM(Bridge::RED, 0, 1000);
    sleep(1);
    this->bridge->setPWM(Bridge::RED, 0, 0);
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            LOG(moba::DEBUG) << "bank: " << i << " color: " << this->bridge->getColorName(Handler::bcolor[j]) << std::endl;
            for(int k = 0; k < 2; ++k) {
                this->bridge->setData(Handler::bcolor[j], i, 0, 150);
                delay(500);
                this->bridge->setData(Handler::bcolor[j], i, 0, 600);
                delay(500);
            }
            this->bridge->setData(Handler::bcolor[j], i, 0, 0);
        }
    }
    this->bridge->setPWM(Bridge::GREEN, 0, 1000);
    sleep(1);
    this->bridge->setAllOff();
    LOG(moba::DEBUG) << "testing... finished!" << std::endl;
}

std::string Handler::getNextOrder() {
    moba::IPC::Message msg;

    while(true) {
/* FIXME
#        if(this->sigTerm->hasSignalTriggered()) {
#            return;
#        }
 */

        if(!this->ipc->receive(msg)) {
            usleep(50000);
            continue;
        }

        switch(msg.mtype) {
            case moba::IPC::EMERGENCY_STOP:
                this->emergency();
                break;

            case moba::IPC::TEST:
                this->test();
                break;

            case moba::IPC::RUN:
                return std::string(msg.mtext);

            default:
                break;
        }
    }
}

void Handler::run() {
    int target[5];
    int step[4];
    long ctr = 0;

    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    moba::IPC::Message msg;

    while(true) {
        std::string data = this->getNextOrder();

        for(int i = 0; i < 5; ++i) {
            found = data.find(';', pos);
            target[i] = atoi(data.substr(pos, found - pos).c_str());
            pos = found + 1;
        }
        LOG(moba::DEBUG) << ctr++ << ": " << (int)(Handler::STEPS * target[4] * 0.01) << " seconds" << std::endl;
        LOG(moba::DEBUG) << "targets blue: " << target[0] << " green: " << target[1] << " red: " << target[2] << " white: " << target[3] << std::endl;

        for(int i = 0; i < 4; ++i) {
            if(!target[4]) {
                this->currRatio[i] = target[i];
                this->bridge->setPWM(Handler::bcolor[i], 0, this->currRatio[i]);
            } else if(target[i] - this->currRatio[i]) {
                step[i] = Handler::STEPS / (target[i] - this->currRatio[i]);
            } else {
                step[i] = 0;
            }
        }

        if(!target[4]) {
            continue;
        }

        for(int i = 1; i <= Handler::STEPS; ++i) {
            delayMicroseconds(target[4] * 10);
            for(int j = 0; j < 4; ++j) {
                if(!step[j] || i % step[j]) {
                    continue;
                }
                if(step[j] > 0 && this->currRatio[j] < Handler::RANGE) {
                    this->currRatio[j]++;
                }
                if(step[j] < 0 && this->currRatio[j] > 0) {
                    this->currRatio[j]--;
                }
                this->bridge->setPWM(Handler::bcolor[j], 0, this->currRatio[j]);
            }

            if(this->sigTerm->hasSignalTriggered()) {
                return;
            }

            if(!this->ipc->receive(msg, moba::IPC::RUN, true)) {
                continue;
            }

            switch(msg.mtype) {
                case moba::IPC::EMERGENCY_STOP:
                    this->emergency();
                    break;

                case moba::IPC::TEST:
                    this->test();
                    break;

                default:
                    break;
            }
        }
        LOG(moba::DEBUG) << "iteration finished" << std::endl;
    }
}

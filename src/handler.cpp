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

namespace {
    const int table[] = {
        0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,
        6, 7, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12,
        12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21,
        22, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
        38, 39, 40, 42, 43, 44, 46, 47, 49, 50, 52, 54, 55, 57, 59, 61, 63, 65,
        67, 69, 71, 73, 75, 78, 80, 83, 86, 88, 91, 94, 97, 100, 103, 106, 110,
        113, 117, 121, 124, 128, 132, 137, 141, 145, 150, 155, 160, 165, 170,
        175, 181, 187, 192, 199, 205, 211, 218, 225, 232, 239, 247, 255, 263,
        271, 280, 289, 298, 307, 317, 327, 338, 348, 359, 371, 382, 394, 407,
        420, 433, 447, 461, 476, 491, 506, 522, 539, 556, 574, 592, 611, 630,
        650, 671, 692, 714, 736, 760, 784, 809, 834, 861, 888, 916, 945, 975,
        1006, 1038, 1071, 1105, 1140, 1176, 1213, 1251, 1291, 1332, 1374, 1418,
        1463, 1509, 1557, 1606, 1657, 1710, 1764, 1820, 1878, 1937, 1998, 2062,
        2127, 2195, 2264, 2336, 2410, 2486, 2565, 2646, 2730, 2817, 2906, 2998,
        3093, 3191, 3292, 3397, 3504, 3615, 3730, 3848, 3970, 4096
    };
}

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

void Handler::emergency() {
    this->bridge->setPWM(Bridge::BLUE,  0, 0);
    this->bridge->setPWM(Bridge::GREEN, 0, 0);
    this->bridge->setPWM(Bridge::RED,   0, 0);
    this->bridge->setPWM(Bridge::WHITE, 0, 4095);
}

void Handler::test() {
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
    sleep(1);
    this->resume();
}

void Handler::resume() {
    for(int j = 0; j < 4; ++j) {
        this->bridge->setPWM(Handler::bcolor[j], 0, this->currRatio[j]);
    }
}

void Handler::run() {
    int step[4];

    while(true) {
        this->fetchNextMsg();
        if(!this->buffer.getItemsCount()) {
            usleep(50000);
            continue;
        }

        TargetValues target = this->buffer.pop();

        for(int i = 0; i < 4; ++i) {
            if(!target.duration) {
                this->currRatio[i] = target.targetIntensity[i];
                this->bridge->setPWM(Handler::bcolor[i], 0, this->currRatio[i]);
            } else if(target.targetIntensity[i] - this->currRatio[i]) {
                step[i] = Handler::STEPS / (target.targetIntensity[i] - this->currRatio[i]);
            } else {
                step[i] = 0;
            }
        }

        if(!target.duration) {
            continue;
        }

        for(int i = 1; i <= Handler::STEPS; ++i) {
            delayMicroseconds(target.duration * 10);
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

            if(!this->fetchNextMsg()) {
                break;
            }
        }
        LOG(moba::DEBUG) << "iteration finished" << std::endl;
    }
}

bool Handler::fetchNextMsg() {
    moba::IPC::Message msg;
    bool emergency = false;
    bool halted    = false;

    while(true) {
        if(this->sigTerm->hasSignalTriggered()) {
            throw HandlerException("sigterm catched");
        }

        if(!this->ipc->receive(msg)) {
            if(emergency || halted) {
                usleep(50000);
                continue;
            }
            return true;
        }

        switch(msg.mtype) {
            case moba::IPC::CMD_EMERGENCY_STOP:
                LOG(moba::DEBUG) << "emergency..." << std::endl;
                if(emergency) {
                    LOG(moba::WARNING) << "emergency allready set!" << std::endl;
                    break;
                }
                this->emergency();
                emergency = true;
                break;

            case moba::IPC::CMD_EMERGENCY_RELEASE:
                LOG(moba::DEBUG) << "emergency... off" << std::endl;
                if(emergency) {
                    this->resume();
                    emergency = false;
                } else {
                    LOG(moba::WARNING) << "emergency not set!" << std::endl;
                }
                if(!halted) {
                    return true;
                }
                break;

            case moba::IPC::CMD_HALT:
                LOG(moba::DEBUG) << "halt..." << std::endl;
                if(halted) {
                    LOG(moba::WARNING) << "already halted!" << std::endl;
                    break;
                }
                halted = true;
                break;

            case moba::IPC::CMD_CONTINUE:
                LOG(moba::DEBUG) << "continue..." << std::endl;
                if(!halted) {
                    LOG(moba::WARNING) << "halted not set!" << std::endl;
                }
                halted = false;
                if(!emergency) {
                    return true;
                }
                break;

            case moba::IPC::CMD_TEST:
                LOG(moba::DEBUG) << "testing... " << std::endl;
                if(emergency || halted) {
                    LOG(moba::WARNING) << "no testing! Emergency or halted set" << std::endl;
                    break;
                }
                this->test();
                LOG(moba::DEBUG) << "testing... finished!" << std::endl;
                return true;

            case moba::IPC::CMD_TERMINATE:
                LOG(moba::DEBUG) << "terminate... " << std::endl;
                throw HandlerException("terminate received");
                break;

            case moba::IPC::CMD_RESET:
                LOG(moba::DEBUG) << "reset... " << std::endl;
                this->buffer.reset();
                this->bridge->setAllOff();
                return false;

            case moba::IPC::CMD_RUN:
                LOG(moba::DEBUG) << "run... " << std::endl;
                this->buffer.push(this->parseMessageData(msg.mtext));
                if(!emergency && !halted) {
                    return true;
                }
                break;

            default:
                LOG(moba::WARNING) << "ignoring unknown message-type <" << msg.mtype << ">" << std::endl;
                if(!emergency && !halted) {
                    return true;
                }
                break;
        }
    }
}

Handler::TargetValues Handler::parseMessageData(const std::string &data) {
    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    TargetValues target;

    for(int i = 0; i < 5; ++i) {
        found = data.find(';', pos);
        if(i < 4) {
            target.targetIntensity[i] = atoi(data.substr(pos, found - pos).c_str());
            // FIXME Check if targe valid
            //if(target.targetIntensity[i] > 255)
        } else {
            target.duration = atoi(data.substr(pos, found - pos).c_str());
        }
        pos = found + 1;
    }
    LOG(moba::DEBUG) << ": " << (int)(Handler::STEPS * target.duration * 0.01) << " seconds" << std::endl;
    LOG(moba::DEBUG) <<
            "targets" <<
            " blue: " << target.targetIntensity[0] <<
            " green: " << target.targetIntensity[1] <<
            " red: " << target.targetIntensity[2] <<
            " white: " << target.targetIntensity[3] << std::endl;
    return target;
}
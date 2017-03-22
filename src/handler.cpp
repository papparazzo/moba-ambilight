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

void Handler::emergency() {
    this->bridge->setPWMlg(Bridge::BLUE,  0);
    this->bridge->setPWMlg(Bridge::GREEN, 0);
    this->bridge->setPWMlg(Bridge::RED,   0);
    this->bridge->setPWMlg(Bridge::WHITE, 255);
}

void Handler::test() {
    this->bridge->setPWMlg(Bridge::RED, 211);
    sleep(1);
    this->bridge->setPWMlg(Bridge::RED, 0);
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
    this->bridge->setPWMlg(Bridge::GREEN, 211);
    sleep(1);
    this->bridge->setAllOff();
    sleep(1);
    this->resume();
}

void Handler::resume() {
    for(int j = 0; j < 4; ++j) {
        this->bridge->setPWMlg(Handler::bcolor[j], this->currRatio[j]);
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
                this->bridge->setPWMlg(Handler::bcolor[i], this->currRatio[i]);
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
                this->bridge->setPWMlg(Handler::bcolor[j], this->currRatio[j]);
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
            target.targetIntensity[i] = static_cast<unsigned char>(atoi(data.substr(pos, found - pos).c_str()));
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

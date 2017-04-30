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
#include <sys/time.h>

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
    this->ipc       = ipc;
    this->bridge    = bridge;
    this->sigTerm   = sigTerm;
    this->emergency = false;
    this->halted    = false;

    for(int i = 0; i < 4; ++i) {
        // FIXME this->currRatio[i] = 0;
    }
}

void Handler::run() {
/*
    int i = 0;

    do {
        delayMicroseconds(target.duration);
        for(int j = 0; j < 4; ++j) {
            if(!this->step[j] || i % this->step[j]) {
                continue;
            }
            if(this->step[j] > 0 && this->current.targetIntensity[j] < Handler::RANGE) {
                this->current.targetIntensity[j]++;
            }
            if(this->step[j] < 0 && this->current.targetIntensity[j] > 0) {
                this->current.targetIntensity[j]--;
            }
            this->bridge->setPWMlg(Handler::bcolor[j], this->current.targetIntensity[j]);
        }

        i = i++ % Handler::STEPS;

        if(i) {
            continue;
        }

    } while(true);
 */
}


void Handler::setTargetValues(TargetValues newValues) {
    this->tmp = this->current;

    for(int i = 0; i < 4; ++i) {
        if(!newValues.duration) {
            this->current.targetIntensity[i] = newValues.targetIntensity[i];
            this->bridge->setPWMlg(Handler::bcolor[i], this->current.targetIntensity[i]);
        } else if(newValues.targetIntensity[i] - this->current.targetIntensity[i]) {
            this->step[i] = Handler::STEPS / (newValues.targetIntensity[i] - this->current.targetIntensity[i]);
        } else {
            this->step[i] = 0;
        }
    }
}





void Handler::test() {
    this->bridge->setPWMlg(Bridge::BLUE, 0);
    this->bridge->setPWMlg(Bridge::GREEN, 0);
    this->bridge->setPWMlg(Bridge::WHITE, 0);
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
    sleep(1);/*
    for(int j = 0; j < 4; ++j) {
        this->bridge->setPWMlg(Handler::bcolor[j], this->currRatio[j]);
    }
    */
}




void getNextTarget() {

}




bool Handler::fetchNextMsg() {

    if(!this->buffer.getItemsCount()) {
        usleep(50000);
        //continue;
    }

    //TargetValues target = this->buffer.pop();
    TargetValues target;


    moba::IPC::Message msg;

    while(true) {
        if(this->sigTerm->hasAnySignalTriggered()) {
            throw HandlerException("sigterm catched");
        }

        if(!this->ipc->receive(msg)) {
            if(this->emergency || this->halted) {
                usleep(50000);
                continue;
            }
            return true;
        }

        switch(msg.mtype) {
            case moba::IPC::CMD_EMERGENCY_STOP:
                LOG(moba::DEBUG) << "emergency..." << std::endl;
                if(this->emergency) {
                    LOG(moba::WARNING) << "emergency allready set!" << std::endl;
                    break;
                }
                TargetValues target;
                target.targetIntensity[0] = 0;
                /*
                        ... // FIXME
                        ...
                        ...
                 */
                this->setTargetValues(target);
                this->emergency = true;
                break;

            case moba::IPC::CMD_EMERGENCY_RELEASE:
                LOG(moba::DEBUG) << "emergency... off" << std::endl;
                if(this->emergency) {
                    this->setTargetValues(this->tmp);
                    this->emergency = false;
                } else {
                    LOG(moba::WARNING) << "emergency not set!" << std::endl;
                }
                if(!this->halted) {
                    return true;
                }
                break;

            case moba::IPC::CMD_HALT:
                LOG(moba::DEBUG) << "halt..." << std::endl;
                if(this->halted) {
                    LOG(moba::WARNING) << "already halted!" << std::endl;
                    break;
                }
                this->halted = true;
                break;

            case moba::IPC::CMD_CONTINUE:
                LOG(moba::DEBUG) << "continue..." << std::endl;
                if(!this->halted) {
                    LOG(moba::WARNING) << "halted not set!" << std::endl;
                }
                this->halted = false;
                if(!this->emergency) {
                    return true;
                }
                break;

            case moba::IPC::CMD_TEST:
                LOG(moba::DEBUG) << "testing... " << std::endl;
                if(this->emergency || this->halted) {
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
                if(!this->emergency && !this->halted) {
                    return true;
                }
                break;

            default:
                LOG(moba::WARNING) << "ignoring unknown message-type <" << msg.mtype << ">" << std::endl;
                if(!this->emergency && !this->halted) {
                    return true;
                }
                break;
        }
    }
}

Handler::TargetValues Handler::parseMessageData(const std::string &data) {
    static int counter = 0;
    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    TargetValues target;
    int val;

    for(int i = 0; i < 5; ++i) {
        found = data.find(';', pos);
        if(i < 4) {
            val = atoi(data.substr(pos, found - pos).c_str());
            if(val < 0) {
                val = 0;
            }
            if(val > 4095) {
                val = 4095;
            }
            target.targetIntensity[i] = val;
        } else {
            target.duration = atoi(data.substr(pos, found - pos).c_str());
        }
        pos = found + 1;
    }
    target.counter = counter++;
    LOG(moba::DEBUG) << "--> " << "inserting #" << target.counter << "..." << std::endl;

    LOG(moba::DEBUG) <<
            "--> duration: ~" << target.duration <<
            " sec. (~" << (int)(target.duration / 60) << " min.)" << std::endl;
    LOG(moba::DEBUG) <<
            "--> targets" <<
            " blue: " << target.targetIntensity[0] <<
            " green: " << target.targetIntensity[1] <<
            " red: " << target.targetIntensity[2] <<
            " white: " << target.targetIntensity[3] << std::endl;
    target.duration = static_cast<int>((target.duration * 1000 * 1000) / Handler::STEPS);
    return target;
}

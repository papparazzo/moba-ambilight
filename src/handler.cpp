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

#include "handler.h"

#include <moba/log.h>
#include <moba/ringbuffer.h>

#include <wiringPi.h>
#include <string.h>
#include <sys/time.h>

Handler::Handler(
    boost::shared_ptr<Bridge> bridge,
    boost::shared_ptr<moba::IPC> ipc,
    boost::shared_ptr<moba::SignalHandler> sigTerm
) : ipc(ipc), bridge(bridge), sigTerm(sigTerm), emergency(false) {
    duration = DEFAULT_DURATION;
}

void Handler::run() {
    do {
        fetchNextMsg();
        /*
    if(!regularBuffer.hasItems()) {
        return false;
    }

    ProcessData regular = regularBuffer.pop();
    ProcessData interrupt;

    int i;
    do {
        ++i;
        //delayMicroseconds(duration + current.duration);
        if(i % regular.duration) {
            stepRegular();
        }
        if(interrupted && i % interrupt.duration) {
            stepInterrupt();
        }
    } while(true);

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


         *
         *
         * if(!controller->next()) {
            usleep(50000);
            continue;
        }
        */
    } while(true);
}

void Handler::fetchNextMsg() {
    moba::IPC::Message msg;
    bool halted = false;

    while(true) {
        if(sigTerm->hasAnySignalTriggered()) {
            throw HandlerException("sig-term caught");
        }

        if(!ipc->receive(msg)) {
            if(emergency) {
                return;
            }
            if(!halted) {
                return;
            }
            usleep(50000);
            continue;
        }

        switch(msg.mtype) {
            /*
            case moba::IPC::CMD_EMERGENCY_STOP: {
                LOG(moba::DEBUG) << "emergency..." << std::endl;
                if(emergency) {
                    LOG(moba::WARNING) << "emergency already set!" << std::endl;
                    break;
                }
                runEmergencyMode(msg.mtext);
                emergency = true;
                break;
            }

            case moba::IPC::CMD_EMERGENCY_RELEASE: {
                LOG(moba::DEBUG) << "emergency... off" << std::endl;
                if(!emergency) {
                    LOG(moba::WARNING) << "emergency not set!" << std::endl;
                    break;
                }
                controller->releaseEmergencyStop();
                emergency = false;
                break;
            }
            */
            case moba::IPC::CMD_TEST: {
                LOG(moba::DEBUG) << "testing... " << std::endl;
                runTestMode();
                LOG(moba::DEBUG) << "testing... finished!" << std::endl;
                return;
            }

            case moba::IPC::CMD_RUN: {
                LOG(moba::DEBUG) << "run... " << std::endl;
                insertNext(msg.mtext);
                break;
            }

            case moba::IPC::CMD_HALT: {
                LOG(moba::DEBUG) << "halt..." << std::endl;
                if(halted) {
                    LOG(moba::WARNING) << "already halted!" << std::endl;
                }
                halted = true;
                break;
            }

            case moba::IPC::CMD_CONTINUE: {
                LOG(moba::DEBUG) << "continue..." << std::endl;
                if(!halted) {
                    LOG(moba::WARNING) << "halted not set!" << std::endl;
                }
                halted = false;
                break;
            }

            case moba::IPC::CMD_RESET: {
                LOG(moba::DEBUG) << "reset... " << std::endl;
                reset(msg.mtext);
                halted = false;
                break;
            }

            case moba::IPC::CMD_TERMINATE: {
                LOG(moba::DEBUG) << "terminate... " << std::endl;
                throw HandlerException("terminate received");
            }
            /*
            case moba::IPC::CMD_INTERRUPT: {
                LOG(moba::DEBUG) << "interrupt... " << std::endl;
                controller->setNewTarget(parseMessageData(msg.mtext), true);
                break;
            }

            case moba::IPC::CMD_RESUME: {
                LOG(moba::DEBUG) << "resume... " << std::endl;
                controller->resume();
                break;
            }
            */
            case moba::IPC::CMD_SET_DURATION: {
                LOG(moba::DEBUG) << "set duration... " << std::endl;
                duration = atoi(msg.mtext);
                LOG(moba::DEBUG) <<
                    "--> duration: ~" << duration <<
                    " sec. (~" << (int)(duration / 60) << " min.)" << std::endl;
                duration = static_cast<int>((duration * 1000 * 1000) / Handler::TOTAL_STEPS_COUNT);
                break;
            }

            default:
                LOG(moba::WARNING) << "ignoring unknown message-type <" << msg.mtype << ">" << std::endl;
                break;
        }
        if(!halted || emergency) {
            return;
        }
    }
}

void Handler::runTestMode() {
    const Bridge::BankColor bcolor[] = {
        Bridge::BLUE,
        Bridge::GREEN,
        Bridge::RED,
        Bridge::WHITE
    };

    bridge->setPWMlg(Bridge::BLUE, 0);
    bridge->setPWMlg(Bridge::GREEN, 0);
    bridge->setPWMlg(Bridge::WHITE, 0);
    bridge->setPWMlg(Bridge::RED, 211);
    sleep(1);
    bridge->setPWMlg(Bridge::RED, 0);
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 2; ++k) {
                bridge->setData(bcolor[j], i, 0, 150);
                delay(500);
                bridge->setData(bcolor[j], i, 0, 600);
                delay(500);
            }
            bridge->setData(bcolor[j], i, 0, 0);
        }
    }
    bridge->setPWMlg(Bridge::GREEN, 211);
    sleep(1);
    bridge->setAllOff();
    sleep(1);
}

void Handler::reset(const std::string &data) {
    Bridge::BankColorValues values;
    if(data != "") {
        std::string::size_type pos = 0;
        std::string::size_type found = 0;

        int val;

        for(int i = 0; i < Bridge::COLOR_COUNT ; ++i) {
            found = data.find(';', pos);
            val = atoi(data.substr(pos, found - pos).c_str());
            if(val < Bridge::MIN_VALUE) {
                val = Bridge::MIN_VALUE;
            }
            if(val > Bridge::MAX_VALUE) {
                val = Bridge::MAX_VALUE;
            }
            values.value[i] = val;
            pos = found + 1;
        }
        LOG(moba::DEBUG) <<
            "--> direct" <<
            " white: " << values.value[Bridge::WHITE] <<
            " green: " << values.value[Bridge::GREEN] <<
            " red: " << values.value[Bridge::RED] <<
            " blue: " << values.value[Bridge::BLUE] << std::endl;
    }

    for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
        currentValues[b].value = values;
        bridge->setPWMlg(values, b);
    }
    regularBuffer.reset();
    emergency = false;
}

void Handler::insertNext(const std::string &data) {
    if(data == "") {
        LOG(moba::WARNING) << "no values given!" << std::endl;
        return;
    }



    NewValues values;

    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    int val;

    for(int i = 0; i < 6; ++i) {
        found = data.find(';', pos);
        switch(i) {
            case Bridge::WHITE:
            case Bridge::GREEN:
            case Bridge::RED:
            case Bridge::BLUE:
                val = atoi(data.substr(pos, found - pos).c_str());
                if(val < Bridge::MIN_VALUE) {
                    val = Bridge::MIN_VALUE;
                }
                if(val > Bridge::MAX_VALUE) {
                    val = Bridge::MAX_VALUE;
                }
                for(int j = 0; j < Bridge::BANK_COUNT; ++j) {
                    values.values[i][j] = val;
                }
                break;

            case 5:
                values.duration = atoi(data.substr(pos, found - pos).c_str());
                break;

            case 6:
                switch(data.substr(pos, found - pos)[0]) {
                    case 'W':
                        values.wobble = true;
                        break;

                    default:
                        values.wobble = false;
                        break;
                }
                break;
        }
        pos = found + 1;
    }
    for(int t = 0; t < Bridge::BANK_COUNT; ++t) {
        LOG(moba::DEBUG) <<
            "--> targets " <<
            " white: " << values.values[Bridge::WHITE][t] <<
            " green: " << values.values[Bridge::GREEN][t] <<
            " red: " << values.values[Bridge::RED][t] <<
            " blue: " << values.values[Bridge::BLUE][t] << std::endl;
    }
    LOG(moba::DEBUG) <<
        "--> wobble: " << (values.wobble ? "on" : "off") << std::endl;
    return values;




    regularBuffer.push(newValues);
    * controller->setNewTarget(parseMessageData(msg.mtext), false);

}





















void Handler::runEmergencyMode(const std::string &data) {
    int duration = EMERGENCY_DURATION;
    int brigthness = EMERGENCY_BRIGTHNESS;

    if(data != "") {
        std::string::size_type pos = 0;
        std::string::size_type found = data.find(';', pos);
        int val = atoi(data.substr(pos, found - pos).c_str());
        if(val < Bridge::MIN_VALUE) {
            val = Bridge::MIN_VALUE;
        }
        if(val > Bridge::MAX_VALUE) {
            val = Bridge::MAX_VALUE;
        }
        brigthness = val;
        if(found != std::string::npos) {
            duration = atoi(data.substr(found + 1).c_str());
        }
    }
    LOG(moba::DEBUG) <<
        "--> targets" <<
        " white: " << brigthness << " duration: " << duration << std::endl;
    controller->emergencyStop(brigthness, duration);
}


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

#include "processdata.h"
#include "processdatahold.h"
#include "processdataplain.h"
#include "processdatawobble.h"

#include <moba/log.h>
#include <moba/ringbuffer.h>

#include <wiringPi.h>
#include <string.h>
#include <sys/time.h>

Handler::Handler(
    boost::shared_ptr<Bridge> bridge,
    boost::shared_ptr<moba::IPC> ipc,
    boost::shared_ptr<moba::SignalHandler> sigTerm
) : ipc(ipc), bridge(bridge), sigTerm(sigTerm), emergency(false), interuptMode(false) {
    duration = DEFAULT_DURATION;
}

void Handler::run() {
    do {
        fetchNextMsg();

        if(!regularBuffer.hasItems()) {
            usleep(50000);
            continue;
        }

        boost::shared_ptr<ProcessData> next = regularBuffer.pop();
        int interruption = next->getInterruptionTime();
        int i = 0;
        do {
            if(++i % 1000) {
                fetchNextMsg();
            }
            delayMicroseconds(interruption);
            if(emergency) {
                continue;
            }

            if(!next->hasNext(!interuptMode)) {
                break;
            }
        } while(true);
    } while(true);
}

void Handler::fetchNextMsg() {
    moba::IPC::Message msg;

    while(true) {
        if(sigTerm->hasAnySignalTriggered()) {
            throw HandlerException("sig-term caught");
        }

        if(!ipc->receive(msg)) {
            if(!emergency) {
                return;
            }
            usleep(50000);
            continue;
        }

        switch(msg.mtype) {
            case moba::IPC::CMD_EMERGENCY_STOP: {
                LOG(moba::DEBUG) << "CMD_EMERGENCY_STOP..." << std::endl;
                if(emergency) {
                    LOG(moba::WARNING) << "emergency already set!" << std::endl;
                    break;
                }
                runEmergencyMode(msg.mtext);
                emergency = true;
                break;
            }

            case moba::IPC::CMD_EMERGENCY_RELEASE: {
                LOG(moba::DEBUG) << "CMD_EMERGENCY_RELEASE..." << std::endl;
                if(!emergency) {
                    LOG(moba::WARNING) << "emergency not set!" << std::endl;
                    break;
                }
                releaseEmergencyStop();
                emergency = false;
                break;
            }

            case moba::IPC::CMD_TEST: {
                LOG(moba::DEBUG) << "CMD_TEST..." << std::endl;
                runTestMode();
                LOG(moba::DEBUG) << "testing finished!" << std::endl;
                return;
            }

            case moba::IPC::CMD_RUN: {
                LOG(moba::DEBUG) << "CMD_RUN..." << std::endl;
                insertNext(msg.mtext);
                break;
            }

            case moba::IPC::CMD_RESET: {
                LOG(moba::DEBUG) << "CMD_RESET..." << std::endl;
                reset(msg.mtext);
                break;
            }

            case moba::IPC::CMD_TERMINATE: {
                LOG(moba::DEBUG) << "CMD_TERMINATE..." << std::endl;
                throw HandlerException("terminate received");
            }

            case moba::IPC::CMD_INTERRUPT: {
                LOG(moba::DEBUG) << "CMD_INTERRUPT..." << std::endl;
               // controller->setNewTarget(parseMessageData(msg.mtext), true);
                break;
            }

            case moba::IPC::CMD_RESUME: {
                LOG(moba::DEBUG) << "CMD_RESUME..." << std::endl;
                //controller->resume();
                break;
            }

            case moba::IPC::CMD_SET_DURATION: {
                LOG(moba::DEBUG) << "CMD_SET_DURATION... " << std::endl;
                duration = atoi(msg.mtext);
                LOG(moba::DEBUG) <<
                    "duration total: ~" << duration <<
                    " sec. (~" << (int)(duration / 60) << " min.)" << std::endl;
                break;
            }

            default:
                LOG(moba::WARNING) << "ignoring unknown message-type <" << msg.mtype << ">" << std::endl;
                break;
        }
        if(emergency) {
            return;
        }
    }
}

void Handler::runTestMode() {
    const BankColorValues::BankColor bcolor[] = {
        BankColorValues::BLUE,
        BankColorValues::GREEN,
        BankColorValues::RED,
        BankColorValues::WHITE
    };

    bridge->setPWMlg(BankColorValues::BLUE, 0);
    bridge->setPWMlg(BankColorValues::GREEN, 0);
    bridge->setPWMlg(BankColorValues::WHITE, 0);
    bridge->setPWMlg(BankColorValues::RED, 211);
    sleep(1);
    bridge->setPWMlg(BankColorValues::RED, 0);
    for(int b = 0; b < 4; ++b) {
        LOG(moba::DEBUG) << "testing bank <" << b << ">" << std::endl;
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 2; ++k) {
                bridge->setData(bcolor[j], b, 0, 150);
                delay(500);
                bridge->setData(bcolor[j], b, 0, 600);
                delay(500);
            }
            bridge->setData(bcolor[j], b, 0, 0);
        }
    }
    bridge->setPWMlg(BankColorValues::GREEN, 211);
    sleep(1);
    bridge->setAllOff();
    sleep(1);
}

void Handler::reset(const std::string &data) {
    BankColorValues values;
    if(data != "") {
        std::string::size_type pos = 0;
        std::string::size_type found = 0;

        int val;

        for(int i = 0; i < BankColorValues::COLOR_COUNT ; ++i) {
            found = data.find(';', pos);
            val = atoi(data.substr(pos, found - pos).c_str());
            values.setValue(i, val);
            pos = found + 1;
        }
    }
    currentValues.setAll(values);
    bridge->setPWMlg(values);
    regularBuffer.reset();
    emergency = false;
}

void Handler::insertNext(const std::string &data) {
    LOG(moba::DEBUG) << "data: <" << data << ">" << std::endl;
    if(data == "") {
        LOG(moba::WARNING) << "no values given!" << std::endl;
        return;
    }

    BankColorValues values;

    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    int durationOverride = duration;
    int val;

    for(int i = 0; i < 6; ++i) {
        found = data.find(';', pos);
        switch(i) {
            case BankColorValues::WHITE:
            case BankColorValues::GREEN:
            case BankColorValues::RED:
            case BankColorValues::BLUE:
                val = atoi(data.substr(pos, found - pos).c_str());
                for(int b = 0; b < Bridge::BANK_COUNT; ++b) {
                    values.setValue(b, i, val);
                }
                break;

            case 4:
                durationOverride = atoi(data.substr(pos, found - pos).c_str());
                break;
        }
        pos = found + 1;
    }
    boost::shared_ptr<ProcessData> item(new ProcessData(bridge, currentValues, values, durationOverride));

    currentValues.setAll(values);
    regularBuffer.push(item);
}

void Handler::runEmergencyMode(const std::string &data) {
    int duration = EMERGENCY_DURATION;
    int brightness = EMERGENCY_BRIGHTNESS;

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
        brightness = val;
        if(found != std::string::npos) {
            duration = atoi(data.substr(found + 1).c_str());
        }
    }
    //controller->emergencyStop(brightness, duration);
}

void Handler::releaseEmergencyStop() {

}

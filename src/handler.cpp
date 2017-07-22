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
#include <wiringPi.h>
#include <string.h>
#include <sys/time.h>

Handler::Handler(
    boost::shared_ptr<Controller> controller,
    boost::shared_ptr<moba::IPC> ipc,
    boost::shared_ptr<moba::SignalHandler> sigTerm
) : ipc(ipc), sigTerm(sigTerm), controller(controller), emergency(false) {
}

void Handler::run() {
    do {
        fetchNextMsg();
        if(!controller->next()) {
            usleep(50000);
            continue;
        }

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

            case moba::IPC::CMD_TEST: {
                LOG(moba::DEBUG) << "testing... " << std::endl;
                controller->runTestMode();
                LOG(moba::DEBUG) << "testing... finished!" << std::endl;
                return;
            }

            case moba::IPC::CMD_RUN: {
                LOG(moba::DEBUG) << "run... " << std::endl;
                controller->setNewTarget(parseMessageData(msg.mtext), false);
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
                controller->reset();
                controller->setDirectValues(parseDirectMessageData(msg.mtext));
                emergency = false;
                halted = false;
                break;
            }

            case moba::IPC::CMD_TERMINATE: {
                LOG(moba::DEBUG) << "terminate... " << std::endl;
                throw HandlerException("terminate received");
            }

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

            case moba::IPC::CMD_SET_DURATION: {
                LOG(moba::DEBUG) << "set duration... " << std::endl;
                int duration = atoi(msg.mtext);
                LOG(moba::DEBUG) <<
                    "--> duration: ~" << duration <<
                    " sec. (~" << (int)(duration / 60) << " min.)" << std::endl;
                controller->setDuration(duration);
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

Bridge::BankColorValues Handler::parseDirectMessageData(const std::string &data) {
    Bridge::BankColorValues values;
    if(data == "") {
        return values;
    }


}

ProcessData Handler::parseMessageData(const std::string &data) {
    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    ProcessData processdata;
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
                processdata.targetIntensity[i] = val;
                break;

            case 5:
                processdata.duration = atoi(data.substr(pos, found - pos).c_str());
                break;

            case 6:
                switch(data.substr(pos, found - pos)[0]) {
                    case 'W':
                        processdata.wobble = true;
                        break;
                }
                break;
        }
        pos = found + 1;
    }
    LOG(moba::DEBUG) << "--> " << "inserting #" << processdata.getObjectId() << "..." << std::endl;
    LOG(moba::DEBUG) <<
        "--> targets" <<
        " white: " << processdata.targetIntensity[Bridge::WHITE] <<
        " green: " << processdata.targetIntensity[Bridge::GREEN] <<
        " red: " << processdata.targetIntensity[Bridge::RED] <<
        " blue: " << processdata.targetIntensity[Bridge::BLUE] << std::endl;
    LOG(moba::DEBUG) <<
        "--> wobble: " << (processdata.wobble ? "on" : "off") << std::endl;
    return target;
}

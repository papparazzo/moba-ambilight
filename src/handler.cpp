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
) : ipc(ipc), bridge(bridge), sigTerm(sigTerm) {
    emergency   = false;
    halted      = false;
    interrupted = false;
}

void Handler::run() {

    int i = 0;
    fetchNextMsg();

/**7
     if(!buffer.getItemsCount()) {
        usleep(50000);
        //continue;
    }

    //TargetValues target = buffer.pop();
    TargetValues target;

 */


    do {
        delayMicroseconds(duration);
        for(int j = 0; j < 4; ++j) {
            if(!step[j] || i % step[j]) {
                continue;
            }
            if(step[j] > 0 && current.targetIntensity[j] < Handler::RANGE) {
                current.targetIntensity[j]++;
            }
            if(step[j] < 0 && current.targetIntensity[j] > 0) {
                current.targetIntensity[j]--;
            }
            bridge->setPWMlg(Handler::bcolor[j], current.targetIntensity[j]);
        }

        i = i++ % Handler::STEPS;

        if(i) {
            continue;
        }

    } while(true);
}

void Handler::setTargetValues(const TargetValues &newValues) {
    for(int i = 0; i < 4; ++i) {
        if(!duration || newValues.direkt) {
            current.targetIntensity[i] = newValues.targetIntensity[i];
            bridge->setPWMlg(Handler::bcolor[i], current.targetIntensity[i]);
        } else if(newValues.targetIntensity[i] - current.targetIntensity[i]) {
            step[i] = Handler::STEPS / (newValues.targetIntensity[i] - current.targetIntensity[i]);
        } else {
            step[i] = 0;
        }
    }
}

void Handler::test() {
    bridge->setPWMlg(Bridge::BLUE, 0);
    bridge->setPWMlg(Bridge::GREEN, 0);
    bridge->setPWMlg(Bridge::WHITE, 0);
    bridge->setPWMlg(Bridge::RED, 211);
    sleep(1);
    bridge->setPWMlg(Bridge::RED, 0);
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            LOG(moba::DEBUG) << "bank: " << i << " color: " << bridge->getColorName(Handler::bcolor[j]) << std::endl;
            for(int k = 0; k < 2; ++k) {
                bridge->setData(Handler::bcolor[j], i, 0, 150);
                delay(500);
                bridge->setData(Handler::bcolor[j], i, 0, 600);
                delay(500);
            }
            bridge->setData(Handler::bcolor[j], i, 0, 0);
        }
    }
    bridge->setPWMlg(Bridge::GREEN, 211);
    sleep(1);
    bridge->setAllOff();
    sleep(1);
}

bool Handler::fetchNextMsg() {
    moba::IPC::Message msg;
    TargetValues tmpTarget;
    int tmpDuration;

    while(true) {
        if(sigTerm->hasAnySignalTriggered()) {
            throw HandlerException("sigterm caught");
        }

        if(!ipc->receive(msg)) {
            if(emergency || halted) {
                usleep(50000);
                continue;
            }
            return true;
        }

        switch(msg.mtype) {
            case moba::IPC::CMD_EMERGENCY_STOP: {
                LOG(moba::DEBUG) << "emergency..." << std::endl;
                if(emergency) {
                    LOG(moba::WARNING) << "emergency already set!" << std::endl;
                    break;
                }
                tmpTarget = current;
                tmpDuration = duration;
                duration = 5;
                TargetValues target = parseMessageData(msg.mtext);
                setTargetValues(target);
                emergency = true;
                break;
            }

            case moba::IPC::CMD_EMERGENCY_RELEASE: {
                LOG(moba::DEBUG) << "emergency... off" << std::endl;
                if(!emergency) {
                    LOG(moba::WARNING) << "emergency not set!" << std::endl;
                    break;
                }
                setTargetValues(tmpTarget);
                duration = tmpDuration;
                emergency = false;
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

            case moba::IPC::CMD_TEST: {
                LOG(moba::DEBUG) << "testing... " << std::endl;
                if(emergency || halted) {
                    LOG(moba::WARNING) << "no testing! Emergency or halted set" << std::endl;
                    break;
                }
                test();
                LOG(moba::DEBUG) << "testing... finished!" << std::endl;
                return true;
            }

            case moba::IPC::CMD_TERMINATE: {
                LOG(moba::DEBUG) << "terminate... " << std::endl;
                throw HandlerException("terminate received");
            }

            case moba::IPC::CMD_RESET: {
                LOG(moba::DEBUG) << "reset... " << std::endl;
                interruptBuffer.reset();
                regularBuffer.reset();
                emergency = false;
                halted = false;
                interrupted = false;
                bridge->setAllOff();
                // FIXME: delete Target ???
                break;
            }

            case moba::IPC::CMD_RUN: {
                LOG(moba::DEBUG) << "run... " << std::endl;
                regularBuffer.push(parseMessageData(msg.mtext));
                break;
            }

            case moba::IPC::CMD_INTERRUPT: {
                LOG(moba::DEBUG) << "interrupt... " << std::endl;
                interrupted = true;
                interruptBuffer.push(parseMessageData(msg.mtext));
                break;
            }

            case moba::IPC::CMD_RESUME: {
                LOG(moba::DEBUG) << "resume... " << std::endl;
                interruptBuffer.reset();
                interrupted = false;
                // TODO: goto default target
                break;
            }

            case moba::IPC::CMD_SET_DURATION: {
                LOG(moba::DEBUG) << "set duration... " << std::endl;
                duration = atoi(msg.mtext);
                LOG(moba::DEBUG) <<
                    "--> duration: ~" << duration <<
                    " sec. (~" << (int)(duration / 60) << " min.)" << std::endl;
                duration = static_cast<int>((duration * 1000 * 1000) / Handler::STEPS);
                break;
            }

            default:
                LOG(moba::WARNING) << "ignoring unknown message-type <" << msg.mtype << ">" << std::endl;
                break;
        }
        if(!emergency && !halted) {
            return true;
        }
    }
}







TargetValues Handler::parseMessageData(const std::string &data) {
    std::string::size_type pos = 0;
    std::string::size_type found = 0;

    TargetValues target;
    int val;

    for(int i = 0; i < 5; ++i) {
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
                target.targetIntensity[i] = val;
                break;

            case 5:
                switch(data.substr(pos, found - pos)[0]) {
                    case 'W':
                        target.wobble = true;
                        break;

                    case 'D':
                        target.direkt = true;
                        break;
                }
                break;
        }
        pos = found + 1;
    }
    LOG(moba::DEBUG) << "--> " << "inserting #" << target.getObjectId() << "..." << std::endl;
    LOG(moba::DEBUG) <<
            "--> targets" <<
            " white: " << target.targetIntensity[Bridge::WHITE] <<
            " green: " << target.targetIntensity[Bridge::GREEN] <<
            " red: " << target.targetIntensity[Bridge::RED] <<
            " blue: " << target.targetIntensity[Bridge::BLUE] << std::endl;
    LOG(moba::DEBUG) <<
            "--> wobble: " << (target.wobble ? "on" : "off") << std::endl;
    return target;
}

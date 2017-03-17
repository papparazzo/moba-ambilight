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

#include <cstdlib>
#include <boost/shared_ptr.hpp>

#include "bridge.h"
#include "handler.h"

#include <moba/ipc.h>
#include <moba/log.h>
#include <moba/signalhandler.h>

int main(int argc, char** argv) {
    int freq = 120;
    int key = moba::IPC::DEFAULT_KEY;

    switch(argc) {
        case 3:
            freq = atoi(argv[2]);

        case 2:
            key = atoi(argv[1]);

        default:
            break;
    }

    LOG(moba::DEBUG) << "Setting PWM frequency to <" << freq << "> Hz" << std::endl;
    LOG(moba::DEBUG) << "Using key <" << key << "> for msg-queue" << std::endl;

    boost::shared_ptr<Bridge> bridge(new Bridge());
    boost::shared_ptr<moba::IPC> ipc(new moba::IPC(key, moba::IPC::SERVER));
    boost::shared_ptr<moba::SignalHandler> sigTerm(new moba::SignalHandler(SIGTERM));

    bridge->setPWMFrequency(freq);

    Handler handler(bridge, ipc, sigTerm);

    try {
        handler.run();
    } catch(std::exception &e) {
        LOG(moba::WARNING) << e.what() << std::endl;
    }
    bridge->setAllOff();
    return EXIT_SUCCESS;
}

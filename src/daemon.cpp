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
#include <getopt.h>
#include <memory>

#include "bridge.h"
#include "handler.h"

#include <moba-common/ipc.h>
#include <moba-common/log.h>
#include <moba-common/helper.h>
#include <moba-common/signalhandler.h>

int main(int argc, char** argv) {
    int freq = 120;
    int key  = moba::common::IPC::DEFAULT_KEY;

    switch(argc) {
        case 3:
            freq = atoi(argv[2]);

        case 2:
            key = atoi(argv[1]);
    }

    LOG(moba::common::LogLevel::DEBUG) << "Using key <" << key << "> for msg-queue" << std::endl;
    LOG(moba::common::LogLevel::DEBUG) << "Setting PWM frequency to <" << freq << "> Hz" << std::endl;

    moba::common::setCoreFileSizeToULimit();

    if(geteuid() != 0) {
        LOG(moba::common::LogLevel::ERROR) << "This daemon can only be run by root user, exiting" << std::endl;
	    exit(EXIT_FAILURE);
	}

    std::shared_ptr<Bridge> bridge(new Bridge());
    std::shared_ptr<moba::common::IPC> ipc(new moba::common::IPC(key, moba::common::IPC::TYPE_SERVER));
    std::shared_ptr<moba::common::SignalHandler> sigTerm(new moba::common::SignalHandler());
    sigTerm->observeSignal(SIGTERM);
    sigTerm->observeSignal(SIGINT);

    bridge->setPWMFrequency(freq);
    Handler handler(bridge, ipc, sigTerm);

    try {
        handler.run();
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    bridge->setAllOff();
    return EXIT_SUCCESS;
}

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
#include <getopt.h>

#include "bridge.h"
#include "handler.h"

#include <config.h>

#include <moba/ipc.h>
#include <moba/log.h>
#include <moba/helper.h>
#include <moba/signalhandler.h>

namespace {
    moba::AppData appData = {
        PACKAGE_NAME,
        moba::Version(PACKAGE_VERSION),
        __DATE__,
        __TIME__,
        "",
        0
    };
}

int main(int argc, char** argv) {




    int freq = 120;
    int key = moba::IPC::DEFAULT_KEY;
    std::string data;

    switch(argc) {
        case 3:
            data = std::string(argv[2]);

        case 2:
            key = atoi(argv[1]);

        default:
            break;
    }


    LOG(moba::DEBUG) << "Using key <" << key << "> for msg-queue" << std::endl;
    LOG(moba::DEBUG) << "Setting PWM frequency to <" << freq << "> Hz" << std::endl;

    boost::shared_ptr<Bridge> bridge(new Bridge());
    boost::shared_ptr<moba::IPC> ipc(new moba::IPC(key, moba::IPC::TYPE_SERVER));
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




/*

void printHelp() {

}

void sendData() {
    if(data != "") {
        moba::IPC ipc(key, moba::IPC::TYPE_CLIENT);

        ipc.send(data, moba::IPC::CMD_RUN);
        ipc.send("0;0;0;0;1", moba::IPC::CMD_RUN);
        return EXIT_SUCCESS;
    }
}
* /

void parseParams(int argc, char **argv, int &freq, int &key) {

}

int main (int argc, char **argv) {

    int freq = 120;
    int key = moba::IPC::DEFAULT_KEY;
    std::string data;

    static struct option longOptions[] = {
        {"key",       required_argument, 0, 'k'},
        {"frequency", required_argument, 0, 'f'},
        {"data",      required_argument, 0, 'd'},
        {"action",    required_argument, 0, 'a'},
        {"help",      no_argument,       0, 'h'},
        {"version",   no_argument,       0, 'v'},
        {NULL,      0, NULL, 0}
    };

    int optionIndex = 0;

    while (true) {
        int c = getopt_long(argc, argv, "k:f:d:a:hv", longOptions, &optionIndex);
        if(c == -1) {
            break;
        }

        switch (c) {
            case 'k':
                LOG(moba::DEBUG) << "k " << optarg << std::endl;
                break;

            case 'f':
                LOG(moba::DEBUG) << "f " << optarg << std::endl;
                break;

            case 'd':
                LOG(moba::DEBUG) << "d " << optarg << std::endl;
                break;

            case 'a':
                LOG(moba::DEBUG) << "a " << optarg << std::endl;
                break;

            case 'h':
                LOG(moba::DEBUG) << "h " << optarg << std::endl;
                break;

            case 'v':
                moba::printAppData(appData);
                break;
        }
    }
    exit (0);
}
 //* */
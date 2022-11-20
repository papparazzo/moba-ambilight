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
#include <exception>
#include <iostream>

#include <config.h>

#include <moba-common/ipc.h>
#include <moba-common/helper.h>
#include <moba-common/signalhandler.h>

namespace {
    moba::common::AppData appData = {
        PACKAGE_NAME,
        moba::common::Version{PACKAGE_VERSION},
        __DATE__,
        __TIME__,
        "",
        0
    };

    struct CmdLineArguments {
        int key;
        moba::common::IPC::Command action;
        std::string data;
    };
}

class ParseCmdLineException : public std::exception {

    public:
        explicit ParseCmdLineException(const std::string &err) throw() : what_(err) {
        }

        ParseCmdLineException() throw() : what_("Unknown error") {
        }

        virtual ~ParseCmdLineException() throw() {
        }

        virtual const char *what() const throw() {
            return what_.c_str();
        }

    private:
        std::string what_;
};

void printHelp() {
    std::cout << std::endl;
    std::cout <<
        "-a, --action    " <<
        "[EMERGENCY_STOP] | [EMERGENCY_RELEASE] | [TEST] | [RUN] | " <<
        "[HALT] | [CONTINUE] | [RESET] | [TERMINATE] | [INTERRUPT] | " <<
        "[RESUME] | [SET_DURATION]" << std::endl;
    std::cout << "-k, --key       ipc communication-key (default: " << moba::common::IPC::DEFAULT_KEY << ")" << std::endl;
    std::cout << "-d, --data      [white];[green];[blue];[red];[wobble{W}]" << std::endl;
    std::cout << "-h, --help      shows this help" << std::endl;
    std::cout << "-v, --version   shows version-info" << std::endl;
    std::cout << std::endl << std::endl << std::endl;
    std::cout << "action               data               optional           description       " << std::endl;
    std::cout << "============================================================================ " << std::endl;
    std::cout << "EMERGENCY_STOP       [0-4095];[1-100]      X            brightness;duration  " << std::endl;
    std::cout << "EMERGENCY_RELEASE    -                     -                                 " << std::endl;
    std::cout << "TEST                 -                     -                                 " << std::endl;
    std::cout << "RUN                  -                     -                                 " << std::endl;
    std::cout << "HALT                 -                     -                                 " << std::endl;
    std::cout << "CONTINUE             -                     -                                 " << std::endl;
    std::cout << "RESET                                                                        " << std::endl;
    std::cout << "TERMINATE                                                                    " << std::endl;
    std::cout << "INTERRUPT                                                                    " << std::endl;
    std::cout << "RESUME                                                                       " << std::endl;
    std::cout << "SET_DURATION                                                                 " << std::endl;
}

bool parseArguments(int argc, char** argv, CmdLineArguments &args) {
    static struct option longOptions[] = {
        {"key",       required_argument, 0,    'k'},
        {"data",      required_argument, 0,    'd'},
        {"action",    required_argument, 0,    'a'},
        {"help",      no_argument,       0,    'h'},
        {"version",   no_argument,       0,    'v'},
        {NULL,        0,                 NULL, 0  }
    };

    int optionIndex = 0;

    while (true) {
        int c = getopt_long(argc, argv, "k:d:a:hv", longOptions, &optionIndex);
        if(c == -1) {
            break;
        }

        switch (c) {
            case 'k':
                args.key = atoi(optarg);
                break;

            case 'd':
                args.data = std::string(optarg);
                break;

            case 'a':
                args.action = moba::common::IPC::getCMDFromString(optarg);
                break;

            case 'h':
                printHelp();
                return true;

            case 'v':
                moba::common::printAppData(appData);
                return true;
        }
    }
    return false;
}

int main(int argc, char** argv) {
    CmdLineArguments args = {
        moba::common::IPC::DEFAULT_KEY,
        moba::common::IPC::CMD_RUN,
        ""
    };

    try {
        if(parseArguments(argc, argv, args)) {
            return EXIT_SUCCESS;
        }
        moba::common::IPC ipc(args.key, moba::common::IPC::TYPE_CLIENT);
        std::cout << "sending <" <<  moba::common::IPC::getCMDAsString(args.action) << "> with <" << args.data << "> to " << args.key << std::endl;
        ipc.send(args.data, args.action);
    } catch(std::exception &e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

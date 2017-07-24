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

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <exception>

#include "bridge.h"
#include "processdata.h"

#include <moba/ipc.h>
#include <moba/signalhandler.h>

class HandlerException : public std::exception {

    public:
        virtual ~HandlerException() throw() {

        }

        HandlerException(const std::string &what) {
            what__ = what;
        }

        virtual const char* what() const throw() {
            return what__.c_str();
        }

    private:
        std::string what__;
};

class Handler : private boost::noncopyable {
    public:
        Handler(
            boost::shared_ptr<Bridge> bridge,
            boost::shared_ptr<moba::IPC> ipc,
            boost::shared_ptr<moba::SignalHandler> sigTerm
        );

        void run();

    protected:
        static const int RANGE  = 4095;
        static const int TOTAL_STEPS_COUNT  = RANGE * 10;

        static const int EMERGENCY_BRIGTHNESS = 2000;
        static const int EMERGENCY_DURATION   = 5;

        static const int DEFAULT_DURATION     = 0;

        boost::shared_ptr<Bridge> bridge;
        boost::shared_ptr<moba::IPC> ipc;
        boost::shared_ptr<moba::SignalHandler> sigTerm;

        void fetchNextMsg();

        void runTestMode();

        void runEmergencyMode(const std::string &data);
        ProcessData parseMessageData(const std::string &data);
        Bridge::BankColorValues parseDirectMessageData(const std::string &data);

        bool emergency;
        int duration;

        moba::Ringbuffer<ProcessData> regularBuffer;
};

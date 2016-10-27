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

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "bridge.h"

#include <moba/ipc.h>
#include <moba/signalhandler.h>

class Handler : private boost::noncopyable {
    public:
        Handler(
            boost::shared_ptr<Bridge> bridge,
            boost::shared_ptr<moba::IPC> ipc,
            boost::shared_ptr<moba::SignalHandler> sigEmergency,
            boost::shared_ptr<moba::SignalHandler> sigContinue
        );

        void run();
        void test();

    protected:
        boost::shared_ptr<Bridge> bridge;
        boost::shared_ptr<moba::IPC> ipc;
        boost::shared_ptr<moba::SignalHandler> sigEmergency;
        boost::shared_ptr<moba::SignalHandler> sigContinue;

        void emergency();
        void emergencyOn();
        void emergencyOff();

        static const int RANGE  = 4095;
        static const int STEPS  = RANGE * 10;

        int currRatio[4];

        const static Bridge::BankColor bcolor[];
};

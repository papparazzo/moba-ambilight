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

#include <exception>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "bridge.h"
#include "targetvalues.h"

#include <moba/ringbuffer.h>

class ControllerException : public std::exception {

    public:
        virtual ~ControllerException() throw() {
        }

        ControllerException(const std::string &what) {
            what__ = what;
        }

        virtual const char* what() const throw() {
            return what__.c_str();
        }

    private:
        std::string what__;
};


class Controller : private boost::noncopyable {
    public:
        Controller(boost::shared_ptr<Bridge> b);
        virtual ~Controller();

        void setNewTarget(const TargetValues &newValues, bool immediately);
        //void runEmergencyMode(const std::string &data);
        bool next();
        void reset();
        void setDuration(int duration);
        void runTestMode();
        void resume();

        void emergencyStop();
        void emergencyStop(const TargetValues &val);
        void releaseEmergencyStop();

    private:

        void setNextTarget(const TargetValues &newValues);

        TargetValues prefetch(int step = 0);

        static const int RANGE  = 4095;
        static const int STEPS  = RANGE * 10;

        static const int EMERGENCY_BRIGTHNESS = 2000;
        static const int EMERGENCY_DURATION   = 5;

        static const int DEFAULT_DURATION     = 0;


        TargetValues current;
        int step[4];

        const static Bridge::BankColor bcolor[];


        int duration;


        boost::shared_ptr<Bridge> bridge;

        moba::Ringbuffer<TargetValues> regularBuffer;
        moba::Ringbuffer<TargetValues> interruptBuffer;


};


class Plasma {

    public:


        void setAmlitudeAndOffset(Bridge::BankColor color, int amplitude, int offset);

    private:


        double d(int x, double t);

        int counter;

        struct Range {
            int amplitude;
            int offset;
        };

        Range range[Bridge::COLOR_COUNT];
};

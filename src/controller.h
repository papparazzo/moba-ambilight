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

        void setNextTarget(const TargetValues &newValues);
        bool nextStep();

        //void setDuration(int duration);
        //void reset();

    private:
        boost::shared_ptr<Bridge> bridge;

};


class Plasma {

    public:

        void next();
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
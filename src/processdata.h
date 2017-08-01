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

#include "bridge.h"
#include "bankcolorvalues.h"

class ProcessDataException : public std::exception {

    public:
        virtual ~ProcessDataException() throw() {
        }

        ProcessDataException(const std::string &what) {
            what__ = what;
        }

        virtual const char* what() const throw() {
            return what__.c_str();
        }

    private:
        std::string what__;
};

class ProcessData {
    public:
        static const int RANGE = Bridge::MAX_VALUE;
        static const int TOTAL_STEPS_COUNT = RANGE * 10;

        ProcessData(boost::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, unsigned int dur = 0);

        virtual ~ProcessData();

        virtual BankColorValues getBankColors(int stepsAhead = 1) = 0;
        virtual bool next(bool setOutput) = 0;

        unsigned int getDuration() const;
        unsigned int getObjectId() const;

    protected:
        BankColorValues stepWidth;
        BankColorValues current;
        BankColorValues target;

        boost::shared_ptr<Bridge> bridge;

        unsigned int duration;
        unsigned int counter;

    private:
        unsigned int objNumber;
        static unsigned int objCounter;
};

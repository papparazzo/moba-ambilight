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

#include "bridge.h"
#include <exception>

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

        ProcessData(const Bridge::BankColorValues &start, const Bridge::BankColorValues &end, int dur);
        virtual ~ProcessData();

        unsigned int getObjectId() const;

        Bridge::BankColorValues getBankColors(int stepsAhead);
        bool next();

    protected:
        Bridge::BankColorValues stepWidth;
        Bridge::BankColorValues current;
        Bridge::BankColorValues target;

        int duration;

    private:
        unsigned int objNb;
        static unsigned int counter;
};

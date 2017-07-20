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

class ProcessData {
    public:
        ProcessData();
        virtual ~ProcessData();

        Bridge::BankColorValues[Bridge::BANK_COUNT] stepWidth;
        Bridge::BankColorValues[Bridge::BANK_COUNT] current;
        Bridge::BankColorValues[Bridge::BANK_COUNT] target;

        int duration;

        bool wobble;
        bool direkt;

        unsigned int getObjectId() const;

    private:
        unsigned int objNb;
        static unsigned int counter;
};




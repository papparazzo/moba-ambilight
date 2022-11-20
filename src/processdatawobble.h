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

#include "processdata.h"

class ProcessDataWobble : public ProcessData {
    public:
        ProcessDataWobble(std::shared_ptr<Bridge> bridge, const BankColorValues &start, const BankColorValues &end, const BankColorValues &amp, unsigned int dur = 0);

        virtual unsigned int getBankColors(BankColorValues &values, unsigned int stepsAhead = 1) const;
        virtual bool hasNext(bool setOutput);

    protected:
        BankColorValues amplitude;

    private:
        double getWobbleValue(int b, int c, unsigned int i) const;
};

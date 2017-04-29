#pragma once

#include "bridge.h"
#include <moba/signalhandler.h>
#include <boost/shared_ptr.hpp>

class Plasma {
    public:
        Plasma(
            boost::shared_ptr<Bridge> bridge,
            boost::shared_ptr<moba::SignalHandler> sigTerm
        );

        void run();

    private:
        boost::shared_ptr<Bridge> bridge;
        boost::shared_ptr<moba::SignalHandler> sigTerm;

        double d(int x, double t);
        double d1(int x, double t);
        double d2(int x, double t);
        double d3(int x, double t);
};

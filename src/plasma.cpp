#include "plasma.h"
#include <cmath>
#include <iostream>
#include <wiringPi.h>

#define PI 3.1415926535897932384626433832795

Plasma::Plasma(
    boost::shared_ptr<Bridge> bridge,
    boost::shared_ptr<moba::SignalHandler> sigTerm
) {
    this->bridge    = bridge;
    this->sigTerm   = sigTerm;
}

double Plasma::d(int x, double t) {
    return std::sin(t + 10.0 * x);
}

double Plasma::d1(int x, double t) {
    return std::sin(10.0 * (x * std::sin(t / 2) + std::cos(t / 3)) + t);
}

double Plasma::d2(int x, double t) {
    double cx = x + 0.5 * std::sin(t / 5);
    double cy = 1 + 0.5 * std::cos(t / 3);
    return std::sin(std::sqrt(100.0 * (std::pow(cx, 2) + std::pow(cy, 2)) + 1) + t);
}

double Plasma::d3(int x, double t) {
    double v = d(x, t) + d1(x, t) + d2(x, t);

    double r = 1 + std::sin(v * PI);
    double g = 1 + std::sin(v *  PI + 2 * PI / 3);
    double b = 1 + std::sin(v * PI + 4 * PI / 3);

    std::cerr << "x: " << x << " t: " << t << " ";
    std::cerr << "red: " << (4094 * r) << " green: " << (4094 * g) << " blue: " << (4094 * b) << std::endl;
    this->bridge->setPWMlg(Bridge::RED, (2000 * r));
    this->bridge->setPWMlg(Bridge::GREEN, (2000 * g));
    this->bridge->setPWMlg(Bridge::BLUE, (2000 * b));

}

void Plasma::run() {
    int i = 0;
    do {
        if(this->sigTerm->hasAnySignalTriggered()) {
            return;
        }
        delayMicroseconds(50);
        i = ++i % 1000;
        d3(1, (double)i * PI / 500);
    } while(true);
}

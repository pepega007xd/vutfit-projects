#ifndef INDEPENDENT_TRAIN_H
#define INDEPENDENT_TRAIN_H

#include "passenger.h"
#include "station.h"
#include <simlib.h>
#include <vector>

class IndependentTrain : public Process {
        std::vector<Passenger *> passengers;
        Station *station;

        void Behavior();

    public:
        IndependentTrain(std::vector<Passenger *> p, Station *s) : passengers(p), station(s) {}
};

class IndependentTrainGenerator : public Event {
        Station *station;

        void Behavior();

    public:
        IndependentTrainGenerator(Station *s) : station(s) {}
};

#endif // INDEPENDENT_TRAIN_H

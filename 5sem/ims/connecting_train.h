#ifndef CONNECTING_TRAIN_H
#define CONNECTING_TRAIN_H

#include "passenger.h"
#include <simlib.h>
#include <vector>

class ConnectingTrain : public Process {
        std::vector<Passenger *> passengers;

        void Behavior();
};

#endif // CONNECTING_TRAIN_H

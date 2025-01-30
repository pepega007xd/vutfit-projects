#ifndef PASSENGER_H
#define PASSENGER_H

#include <simlib.h>

class Passenger : public Process {
        Queue *transfer_station;
        void Behavior();

    public:
        int destination_idx;
        Passenger(Queue *s, int d);
};

#endif // PASSENGER_H

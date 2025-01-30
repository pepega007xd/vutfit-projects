#include "passenger.h"
#include "utils.h"

Passenger::Passenger(Queue *s, int d) : transfer_station(s), destination_idx(d) {}

void Passenger::Behavior() {
    auto start_time = Time;

    // Wait for transfer station
    Passivate();

    // Wait for connecting train
    Into(transfer_station);

    // Wait for destination
    Passivate();

    travel_time(Time - start_time);
}

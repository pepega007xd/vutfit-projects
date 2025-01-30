#include "utils.h"
#include "passenger.h"
#include <simlib.h>
#include <vector>

Histogram travel_time("travel_time", 0, 1, 100);
Histogram connecting_train_delay("connecting_train_delay", 0, 1, 15);

/// generates two vectors of passengers with the specified transfer station and
/// random destination station, `count` is the total numrsber of passengers in the vectors
std::pair<std::vector<Passenger *>, std::vector<Passenger *>> get_passengers(Station *station,
                                                                             int count) {
    auto passengers_left = std::vector<Passenger *>();
    auto passengers_right = std::vector<Passenger *>();

    for (int i = 0; i < count; i++) {
        int destination_idx;
        do {
            destination_idx = (int)(Random() * num_stations);
        } while (destination_idx == station->get_index());

        auto idx_diff = destination_idx - station->get_index();

        auto queue =
            idx_diff > 0 ? &station->waiting_passengers_right : &station->waiting_passengers_left;

        auto passenger = new Passenger(queue, destination_idx);
        idx_diff > 0 ? passengers_right.push_back(passenger) : passengers_left.push_back(passenger);
    }

    return {passengers_right, passengers_left};
}

Timeout::Timeout(double t, Process *p) : process(p) { Activate(Time + t); }

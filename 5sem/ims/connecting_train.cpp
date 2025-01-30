#include "connecting_train.h"
#include "params.h"
#include "station.h"
#include "utils.h"

void ConnectingTrain::Behavior() {
    SetName("Connecting Train");

    // starting at the leftmost station
    auto station_idx = 0;

    // 1 means right, -1 means left
    auto direction = 1;

    double delay = 0;

    // this is done to ensure that passengers had time
    // to board the train when arriving without delay
    Wait(0.1);

    while (true) {
        auto &station = stations[station_idx];

        // set new direction of train
        if (station_idx == (int)num_stations - 1) {
            direction = -1;
        } else if (station_idx == 0) {
            direction = 1;
        }

        Println("Connecting Train arrived to station %d at %.2f with delay %.2f", station_idx, Time,
                delay);
        connecting_train_delay(delay);

        double waited = 0;

        if (station.is_arriving.Busy()) {
            // waiting for delayed independent train

            auto start = Time;
            Println("Waiting for train in station %d", station_idx);
            auto timeout = new Timeout(std::max(0.0, max_wait_time - delay), this);
            Seize(station.is_arriving);
            waited = Time - start;
            delete timeout;

            if (station.is_arriving.In() == this) {
                Release(station.is_arriving);
            }
        }

        delay += waited;

        auto &passenger_queue =
            direction == 1 ? station.waiting_passengers_right : station.waiting_passengers_left;

        // onboard passengers waiting in station
        while (!passenger_queue.Empty()) {
            auto passenger = static_cast<Passenger *>(passenger_queue.GetFirst());
            passengers.push_back(passenger);
        }

        // offboard passengers travelling to this station
        for (auto idx = 0ul; idx < passengers.size();) {
            if (passengers[idx]->destination_idx == station_idx) {
                passengers[idx]->Activate();
                passengers.erase(passengers.begin() + (long)idx);
            } else {
                idx++;
            }
        }

        // ride to next station
        double time_to_next = direction == 1 ? station.time_to_next_station
                                             : stations[station_idx - 1].time_to_next_station;

        auto catch_delay = std::min((double)MAX_BOARDING_TIME, delay);
        time_to_next -= catch_delay;
        Wait(std::max(0.0, time_to_next));
        delay -= catch_delay;

        station_idx += direction;
    }
}

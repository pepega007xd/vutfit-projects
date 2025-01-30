#ifndef STATION_H
#define STATION_H

#include "passenger.h"
#include <functional>
#include <simlib.h>

typedef std::function<int()> IntGenerator;
typedef std::function<double()> DoubleGenerator;

extern int num_stations;
extern int connecting_train_period;

struct Station {
        simlib3::Facility is_arriving;
        Queue waiting_passengers_left;
        Queue waiting_passengers_right;

        bool has_independent_train;

        IntGenerator transfer_passengers;
        IntGenerator direct_passengers;

        int repeat_period_multiplier;
        int time_to_next_station;
        DoubleGenerator get_delay;
        int get_index();
        int get_offset();

        Station(IntGenerator transfer, IntGenerator direct, int repeat, int time_to_next,
                DoubleGenerator delay)
            : has_independent_train(true), transfer_passengers(transfer), direct_passengers(direct),
              repeat_period_multiplier(repeat), time_to_next_station(time_to_next),
              get_delay(delay) {}

        Station(IntGenerator direct, int time_to_next)
            : has_independent_train(false), direct_passengers(direct),
              time_to_next_station(time_to_next) {}
};

extern Station stations[];

class StationGenerator : public Event {
        Station *station;

        void Behavior();

    public:
        StationGenerator(Station *s) : station(s) {}
};

class DirectPassengerGenerator : public Process {
        std::vector<Passenger *> passengers;

        void Behavior() {
            for (auto &p : passengers) {
                p->Activate();
            }

            // Yield execution so that passengers can be activated
            Wait(0);

            for (auto &p : passengers) {
                p->Activate();
            }
        }

    public:
        DirectPassengerGenerator(std::vector<Passenger *> p) : passengers(p) {}
};

#endif // STATION_H

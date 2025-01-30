#include "station.h"
#include "params.h"
#include "utils.h"
#include <cassert>
#include <random>
#include <simlib.h>

/// samples delays from delay distribution from https://kam.mff.cuni.cz/~babilon/zpmapa2.html
double get_delay(std::vector<double> delay_stat, double max_delay) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<> dist(delay_stat.begin(), delay_stat.end());

    switch (dist(gen)) {
    case 0:
        return Uniform(0, 5);
    case 1:
        return Uniform(5, 10);
    case 2:
        return Uniform(10, 15);
    case 3:
        return Uniform(15, 20);
    case 4:
        return Uniform(20, 30);
    case 5:
        return Uniform(30, 60);
    default:
        return Uniform(60, max_delay);
    }
}

#define CALLBACK(value) []() { return value; }

// there must be at least two stations
Station stations[] = {
    // TRANSFER STATION:
    // transfer_passengers, direct_passengers, repeat_period, time_to_next_station, delay
    {CALLBACK(Uniform(15, 35)), CALLBACK(Uniform(5, 8)), 5, 20, CALLBACK(DELAY_Ex_74())},
    // SIMPLE STATION:
    // direct_passangers, time_to_next_station
    {CALLBACK(Uniform(5, 9)), 10},
    {CALLBACK(Uniform(10, 15)), CALLBACK(Uniform(4, 14)), 3, 15, CALLBACK(DELAY_Ex_274())},
    {CALLBACK(Uniform(10, 45)), CALLBACK(Uniform(4, 14)), 3, 15, CALLBACK(DELAY_EC_106())},
    {CALLBACK(Uniform(5, 7)), 0},
};

int num_stations = sizeof(stations) / sizeof(stations[0]);

int connecting_train_period = stations[num_stations - 1].get_offset() * 2;

// check if the time_to_next_station fields have valid values
int assert_station_invariants() {
    // there must be at least two stations
    assert(num_stations > 1);

    for (auto &station : stations) {
        if (station.get_index() == num_stations - 1) {
            // last field must be zero (there is no next station)
            assert(station.time_to_next_station == 0);
            return 0;
        }
        // all other fields must be at least MAX_BOARDING_TIME
        // (boarding time is included in time_to_next_station)
        assert(station.time_to_next_station > MAX_BOARDING_TIME);
    }
    return 0;
}
auto _ = assert_station_invariants();

int Station::get_index() { return (int)(this - stations); }

int Station::get_offset() {
    int sum = 0;
    for (auto &station : stations) {
        if (station.get_index() == get_index()) {
            return sum;
        }
        sum += station.time_to_next_station;
    }
    return sum;
}

void StationGenerator::Behavior() {
    auto direct_passengers = get_passengers(station, station->direct_passengers());

    // two groups of direct passengers waiting in the station
    (new DirectPassengerGenerator(direct_passengers.first))->Activate(Time + station->get_offset());
    (new DirectPassengerGenerator(direct_passengers.second))
        ->Activate(Time + connecting_train_period - station->get_offset());

    Activate(Time + connecting_train_period);
}

#include "connecting_train.h"
#include "independent_train.h"
#include "params.h"
#include "station.h"
#include "utils.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <simlib.h>

int max_wait_time = 0;

int main() {
    SetOutput("result.out");
    RandomSeed(std::time(NULL));

    auto step = 5;

    for (max_wait_time = 0; max_wait_time < connecting_train_period + step; max_wait_time += step) {
        Init(0, 100'000); // lowered for execution on merlin, optimal value is 1M to 10M

        travel_time.Clear();

        for (int station_idx = 0; station_idx < num_stations; station_idx++) {
            auto &station = stations[station_idx];

            // clear data from previous simulation
            station.waiting_passengers_left.Clear();
            station.waiting_passengers_right.Clear();
            station.is_arriving.Clear();

            // generate independent train arriving to station
            if (station.has_independent_train) {
                (new IndependentTrainGenerator(&station))->Activate();
            }

            // generate passengers coming directly to station
            (new StationGenerator(&station))->Activate();
        }

        (new ConnectingTrain())->Activate();

        Run();

        // connecting_train_delay.Output();
        Print("max_wait_time = %d, average travel_time: %.3f\n", max_wait_time,
              travel_time.stat.MeanValue());
    }

    return 0;
}

#include "independent_train.h"
#include "station.h"
#include "utils.h"

void IndependentTrain::Behavior() {
    auto start_time = Time;

    // activate passengers only after waiting for offset
    // to synchronize with arrival of connecting train
    for (auto &p : passengers) {
        p->Activate();
    }

    // mark the station as expecting a train
    Seize(station->is_arriving);

    // train is accumulating delay
    Wait(station->get_delay());

    // train has arrived to transfer station
    Release(station->is_arriving);

    // release passengers to transfer station
    for (auto &p : passengers) {
        p->Activate();
    }

    Println("Independent arrived to station %d with delay %.3f", station->get_index(),
            Time - start_time);
}

void IndependentTrainGenerator::Behavior() {
    auto independent_train_passengers = get_passengers(station, station->transfer_passengers());

    // two trains represent two directions of their passengers
    (new IndependentTrain(independent_train_passengers.first, station))
        ->Activate(Time + station->get_offset());
    (new IndependentTrain(independent_train_passengers.second, station))
        ->Activate(Time + connecting_train_period - station->get_offset());

    Activate(Time + station->repeat_period_multiplier * connecting_train_period);
}

#ifndef UTILS_H
#define UTILS_H

#include "passenger.h"
#include "station.h"
#include <simlib.h>
#include <vector>

extern Histogram travel_time;
extern Histogram connecting_train_delay;

template <typename... Args> void Println(Args &&...) {}
// #define Println(...)  { Print(__VA_ARGS__); Print("\n"); }

// removes process out of queue after some time
class Timeout : public Event {
        Process *process;
        void Behavior() {
            if (process->isInQueue()) {
                process->Out();
                process->Activate();
                Println("timeout detected for %s", process->Name().c_str());
            }
        }

    public:
        Timeout(double t, Process *p);
};

std::pair<std::vector<Passenger *>, std::vector<Passenger *>> get_passengers(Station *station,
                                                                             int count);
#endif // UTILS_H

#ifndef PARAMS_H
#define PARAMS_H

// Model parameters

#define INDEPENDENT_MAX_DELAY 120

// https://kam.mff.cuni.cz/~babilon/cgi-bin/zpvlaku.cgi?v=74&d=60
#define DELAY_Ex_74() get_delay({5, 32, 10, 6, 2, 3, 2}, INDEPENDENT_MAX_DELAY)

// https://kam.mff.cuni.cz/~babilon/cgi-bin/zpvlaku.cgi?v=274&d=60
#define DELAY_Ex_274() get_delay({8, 6, 9, 1, 1, 3, 4}, INDEPENDENT_MAX_DELAY)

// https://kam.mff.cuni.cz/~babilon/cgi-bin/zpvlaku.cgi?v=106&d=60
#define DELAY_EC_106() get_delay({10, 1, 4, 7, 5, 5, 0}, INDEPENDENT_MAX_DELAY)

// part of time_to_next_station, can be skipped in ride
// between stations to reduce delay of connecting train
#define MAX_BOARDING_TIME 1.3

// This parameter is configured in each simulation run in `main.cpp`
extern int max_wait_time;

// Stations are configured in `station.cpp`

#endif // PARAMS_H

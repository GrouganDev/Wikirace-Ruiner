#include <chrono>
#include <thread>
#include <iostream>

#ifndef DSA_PROJECT_2_TIMER_H
#define DSA_PROJECT_2_TIMER_H


class Timer {
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
public:
    Timer();

    ~Timer();
};


#endif //DSA_PROJECT_2_TIMER_H
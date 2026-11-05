#include "Timer.h"

Timer::Timer() : m_start_time(std::chrono::steady_clock::now()) {}

Timer::~Timer() {
    auto end_time = std::chrono::steady_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time).count();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

#ifndef UTILS_H
#define UTILS_H

#include <chrono>

/**
 * @brief getNow
 * @return current time point
 */
inline std::chrono::steady_clock::time_point getNow()
{
    return std::chrono::steady_clock::now();
}

/**
 * @brief getDuration
 * @param start
 * @return duration in milliseconds
 */
inline double getDuration(std::chrono::steady_clock::time_point start)
{
    auto cdur = getNow() - start;
    double ret = std::chrono::duration_cast<std::chrono::microseconds>(cdur).count();
    return ret / 1000.;
}

#endif // UTILS_H

#include "TimeTools.h"

namespace u {
namespace reactor {

long TimeTools::getElapsedTime(const struct timespec *start, const struct timespec *end)
{
    long elapsed = 0;
    struct timespec result;

    if (start->tv_nsec > end->tv_nsec) {
        result.tv_sec  = end->tv_sec - start->tv_sec - 1;
        result.tv_nsec = end->tv_nsec - start->tv_nsec + NSEC_PER_SEC;
    } else {
        result.tv_sec  = end->tv_sec - start->tv_sec;
        result.tv_nsec = end->tv_nsec - start->tv_nsec;
    }

    elapsed += result.tv_sec * MSEC_PER_SEC;
    elapsed += result.tv_nsec / NSEC_PER_MSEC;

    return elapsed;
}
} // namespace reactor
} // namespace u

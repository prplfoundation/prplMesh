#ifndef Reactor_TimeTools_H
#define Reactor_TimeTools_H

#include <time.h>

/* Macros used to convert the timespec values: */
#define MSEC_PER_SEC 1000L
#define USEC_PER_MSEC 1000L
#define NSEC_PER_USEC 1000L
#define NSEC_PER_MSEC 1000000L
#define USEC_PER_SEC 1000000L
#define NSEC_PER_SEC 1000000000L
#define FSEC_PER_SEC 1000000000000000LL

namespace u {
namespace reactor {

class TimeTools {
public:
    /**
     * Gets elapsed time from start till end time (in milliseconds).
     * @param start Start time.
     * @param end End time.
     * @return Elapsed time in milliseconds.
     */
    static long getElapsedTime(const struct timespec *start, const struct timespec *end);
};
} // namespace reactor
} // namespace u

#endif

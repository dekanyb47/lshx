#include <time.h>

#include "helper.h"

void hrf_time(char *time_buf, const unsigned int bufsize, const long int rawtime) {
    if (time_buf == NULL || bufsize == 0) return;
    struct tm ts;

    long int local = rawtime;
    ts = *localtime(&local);
    strftime(time_buf, bufsize, "%Y.%m.%d %H:%M:%S %Z", &ts);
}
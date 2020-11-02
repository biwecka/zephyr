#include "accel.h"

void accel_entry_point(void *_num_samples, void *_results, void *_v3) {
    // Create variables with the correct data types
    int *num_samples = _num_samples;
    int *results = _results;

    // Do stuff...
    for (int i = 0; i < *num_samples; i++) {
        results[i] = i;
    }
}
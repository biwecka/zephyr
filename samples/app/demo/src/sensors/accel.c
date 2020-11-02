#include "accel.h"

void accel_entry_point(void *_num_samples, void *_results, void *_v3) {
    // Create variables with the correct data types
    int *num_samples = _num_samples;
    int *results = _results;

    // Do stuff...
    for (int i = 0; i < *num_samples; i++) {
        // X-val
        results[i * 3 + 0] = i + 100;

        // Y-val
        results[i * 3 + 1] = i + 200;

        // Z-val
        results[i * 3 + 2] = i + 300;
    }
}
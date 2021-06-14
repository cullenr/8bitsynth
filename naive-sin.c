#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define PI2 6.28318

int main(void) {
    const double duration      = 1;    // seconds of audio
    const uint32_t sample_rate = 8000; // samples per second
    const uint32_t num_samples = duration * sample_rate;

    // the audio note in Hz that we wish to produce (note A4)
    const double freq = 440;
    // the size of a step in radians to produce our note at our sample rate
    const double step = PI2 * freq / sample_rate;
    // this is our phase accumulator
    double phase = 0;

    for(int i = 0; i < num_samples; i++) {
        // sample the sin function, this will return a value in the range of 
        // 1.0 to -1.0
        const double sample = sin(phase);
        // convert the sample from 1.0 to -1.0 to the range of 255 to 0
        const uint8_t byte = ((sample + 1.0) / 2.0) * 255.0; 
        // write the data to stdout
        printf("%c", byte);

        // increment the phase for the next iteration
        phase += step;
    }

    return 0;
}

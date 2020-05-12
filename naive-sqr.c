#include <math.h>
#include <stdio.h>
#include <stdint.h>

#define PI  3.14159
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

        printf("%c", phase < PI ? 0x00 : 0xFF);

        phase += step;

        if (phase > PI2) 
            phase -= PI2;
    }

    return 0;
}

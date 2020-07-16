#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PI  M_PI
#define PI2 (M_PI * 2)

enum wave_type {SIN, SQR, SAW, TRI};

int main(int argc, char *argv[]) {
    const uint32_t sample_rate = 8000; // samples per second
    double duration            = 1000; // milliseconds of audio
    double freq                = 440;
    enum wave_type wave        = SIN;
    int opt;

    while ((opt = getopt(argc, argv, "d:f:sqtw")) != -1) {
        switch (opt) {
            case 'd': duration = strtod(optarg, NULL); break;
            case 'f': freq     = strtod(optarg, NULL); break;
            case 's': wave     = SIN; break;
            case 'q': wave     = SQR; break;
            case 't': wave     = TRI; break;
            case 'w': wave     = SAW; break;
            default:
                fprintf(stderr, "Usage: %s [-d duration milliseconds]" 
                                "[-f note hz] [-s][-q][-t][-w]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // 0.001 converts milliseconds to seconds
    const uint32_t num_samples = duration * 0.001 * sample_rate;
    const double step = PI2 * freq / sample_rate;
    // this is our phase accumulator
    double phase = 0;

    // start snippet main-loop
    for(int i = 0; i < num_samples; i++) {
        uint8_t byte;
        switch(wave) {
            case SIN:
                // convert the sample from 1.0 to -1.0 to the range of 255 to 0
                byte = ((sin(phase) + 1.0) / 2.0) * 255.0; 
                break;
            case SQR:
                byte = phase < PI ? 0x00 : 0xFF; 
                break;
            case TRI: 
                byte = phase < PI 
                    ? phase / PI * 255.0
                    : 511 - phase / PI * 255.0;
                break;
            case SAW:
                byte = phase / PI2 * 255; 
                break;
            default:
                fprintf(stderr, "unknown wave type! %d", wave);
                exit(EXIT_FAILURE);
        }

        printf("%c", byte);

        phase += step;
        while (phase > PI2) 
            phase -= PI2;
    }
    // end snippet main-loop
    return 0;
}

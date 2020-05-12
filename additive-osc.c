#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PI  M_PI
#define PI2 (M_PI * 2)

enum wave_type {SIN, SQR, SAW, TRI};

/**
 *  Fills the buffer with a given note
 */
void 
tone(const double sample_rate, const double freq, const uint8_t amplitude,
     char *const buf, const size_t len)
{
    fprintf(stderr, "tone: %f %d\n", freq, amplitude);
    const double step = PI2 * freq / sample_rate;
    double phase = 0;

    for (int i = 0; i < len; i++) {
        buf[i] += ((sin(phase) + 1.0) / 2.0) * amplitude;

        phase += step;
        while (phase > PI2) 
            phase -= PI2;
    }
}

int main(int argc, char *argv[])
{
    const double duration      = 1;    // seconds of audio
    const uint32_t sample_rate = 8000; // samples per second
    const uint32_t nyquist     = sample_rate / 2;
    const uint32_t num_samples = duration * sample_rate;

    double freq = 440;
    enum wave_type wave = SIN;
    int opt;

    while ((opt = getopt(argc, argv, "f:sqtw")) != -1) {
        switch (opt) {
            case 'f': freq = strtod(optarg, NULL); break;
            case 's': wave = SIN; break;
            case 'q': wave = SQR; break;
            case 't': wave = TRI; break;
            case 'w': wave = SAW; break;
            default:
                fprintf(stderr, "Usage: %s [-f note hz] [-s][-q][-t][-w]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    //const uint16_t maximal_partial = sample_rate - (nyquist % freq);
    const uint32_t num_partials = sample_rate / freq;
    uint8_t *buf = calloc(num_samples, sizeof(uint8_t));

    fprintf(stderr, "partials: %d\n", num_partials);
    // we could count down from the highest partial and accumuilate amplitudes
    // that way?
    if (wave == SIN) {
        // Sin waves have no partials so we handle them separately to the other
        // shapes.
        tone(sample_rate, freq, 255, buf, num_samples); 
    } else {
        // These waveforms all have partials. Add each partial to the output
        // buffer one at a time.
        for(int i = 1; i < num_partials + 1; i++) {
            switch(wave) {
                case SQR:
                    // multiply by 4/pi to normalise
                    break;
                case TRI: 
                    break;
                case SAW:
                    // we multiply our amplitude weightings by half of the max
                    // values as the sum of 1/2, 1/3, 1/4... approaches 1 plus
                    // the weighting of 1 for the fundamental frequency.
                    tone(sample_rate, i * freq, (1.0 / i) * 127, buf, num_samples); 
                    break;
                default:
                    fprintf(stderr, "unknown wave type! %d", wave);
                    exit(EXIT_FAILURE);
            }
        }
    }

    write(1, buf, num_samples);

    return 0;
}

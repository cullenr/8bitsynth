#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PI  M_PI
#define PI2 (M_PI * 2)

#ifndef DEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif

#define dbg(fmt, ...) \
    do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                            __LINE__, __func__, __VA_ARGS__); } while (0)

enum wave_type {SIN, SQR, SAW, TRI};

/**
 *  Fills the buffer with a sine wave at the given frequency
 */
void 
sine(const double sample_rate, const double freq, const double amplitude,
     float *const buf, const size_t len)
{
    dbg("sine: %f %f\n", freq, amplitude);
    const double step = PI2 * freq / sample_rate;
    double phase = PI;

    for (int i = 0; i < len; i++) {
        buf[i] += sin(phase) * amplitude;

        phase += step;
        while (phase > PI2) 
            phase -= PI2;
    }
}

int
main(int argc, char *argv[])
{
    uint32_t sample_rate    = 8000; // samples per second
    double duration         = 1;    // seconds of audio
    double freq             = 440;  // note frequency
    enum wave_type wave     = SIN;  // wave shape
    uint32_t opt_partials   = 0;    // user override for partial count
    int opt;                        // getopts option

    while ((opt = getopt(argc, argv, "d:f:r:p:sqtw")) != -1) {
        switch (opt) {
            case 'd': duration     = strtod(optarg, NULL); break;
            case 'f': freq         = strtod(optarg, NULL); break;
            case 'r': sample_rate  = atoi(optarg);         break;
            case 'p': opt_partials = atoi(optarg);         break;
            case 's': wave = SIN; break;
            case 'q': wave = SQR; break;
            case 't': wave = TRI; break;
            case 'w': wave = SAW; break;
            default:
                fprintf(stderr, "Usage: %s [-d duration seconds] " 
                        "[-f note hz] [-r sample rate hz] [-p partials n] "
                        "[-s|-q|-t|-w]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const uint32_t nyquist  = sample_rate / 2;
    const uint32_t samples  = duration * sample_rate;
    float *const buf        = calloc(samples, sizeof(float));

    switch(wave) {
        case SIN:
            sine(sample_rate, freq, 1.0, buf, samples); 
            break;
        case SQR:
            {
                // unless overriden, square waves contain only odd harmonics 
                const uint32_t partials = opt_partials > 0 
                    ? opt_partials 
                    : nyquist / freq / 2;

                // note we drop the highest partial
                for (int k = 1; k < partials; k++) {
                    // calculate the mode
                    const uint32_t n = 2 * k - 1;
                    const float amp = 1.0 / n;
                    sine(sample_rate, n * freq, (2.0 /PI) * amp, buf, samples); 
                }
                break;
            }
            break;
        case TRI:
            {
                // unless overriden, triangle waves contain only odd harmonics 
                const uint32_t partials = opt_partials > 0 
                    ? opt_partials 
                    : nyquist / freq / 2;

                // note we drop the highest partial
                for (int k = 1; k < partials; k++) {
                    // calculate the mode
                    const uint32_t n = 2 * k - 1;
                    // we must invert the phase of every other harmonic
                    const float amp = k % 2
                        ? -1.0 / (n * n)        // odd
                        : 1.0 / (n * n);        // even

                    sine(sample_rate, n * freq, (2.0 / PI) * amp, buf, samples); 
                }
                break;
            }
        case SAW:
            {
                // unless overriden, sawtooth waves contain odd and even
                // harmonics.
                const uint32_t partials = opt_partials > 0 
                    ? opt_partials 
                    : nyquist / freq;

                // note we drop the highest partial
                for (int k = 1; k < partials; k++) {
                    // phase should invert for each odd harmonic. We can
                    // effectivly do this by inverting the amplitude.
                    // Switching the assignments here changes the sawtooth
                    // direction.
                    const float amp = k % 2
                        ? -1.0 / k        // odd
                        : 1.0 / k;        // even

                    // the amplitude is further modified according to the
                    // fourier series.
                    sine(sample_rate, k * freq, (2.0 / PI) * amp, buf, samples); 
                }
                break;
            }
        default:
            fprintf(stderr, "unknown wave type! %d", wave);
            exit(EXIT_FAILURE);
    }

    write(1, buf, sizeof buf[0] * samples);
    free(buf);

    return 0;
}

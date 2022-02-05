#include <stdbool.h>
#include <time.h>
#include "../telemetry.h"

// unit test for telemetry.c
void main()
{
    clock_t begin = clock();
    // speed interval array
    const double speed_interval[2] = {0.0, 41.0};
    // test compression and decompression

    // for each longitude and latitude
    for (double speed = speed_interval[0]; speed <= speed_interval[1]; speed += 0.01)
    {

        // print lat and long
        printf("speed: %f", speed);

        // create a compressed string
        char *compressed = compressSpeed(speed);

        // decompress the string
        double speeed = decompressSpeed(compressed);

        printf(" | %f", speeed);

        if (speed < 9.92 && speed >= 0)
        {
            if (fabs(speeed - speed) > 1e-2)
            {
                // if not, throw an error
                printf("\nError: decompressed speed is not the same as the original speed : %f\n", speeed);
                // exit the program
                exit(1);
            }
            else
            {
                printf(" | OK\n");
            }
        }
        else if (speed <= 40.92 && speed >= 0)
        {
            if (fabs(speeed - speed) > 1)
            {
                // if not, throw an error
                printf("\nError: decompressed speed is not the same as the original speed : %f\n", speeed);
                // exit the program
                exit(1);
            }
            else
            {
                printf(" | OK\n");
            }
        }
        else
        {
            if (speeed != 0)
            {
                // if not, throw an error
                printf("\nError: decompressed speed should be zero\n");
                // exit the program
                exit(1);
            }
            else
            {
                printf(" | OK\n");
            }
        }

        
    }
    clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("time spent : %f\n", time_spent);
}

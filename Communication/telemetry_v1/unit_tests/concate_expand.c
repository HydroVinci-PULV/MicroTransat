#include <stdbool.h>
#include <time.h>
#include "../telemetry.h"

// unit test for telemetry.c
void main()
{
    clock_t begin = clock();

    // longitute interval array
    const double lon_interval[2] = {-98.0, 8.0};
    // latitude interval array
    const double lat_interval[2] = {-55.0, 74.0};
    // speed interval array
    const double speed_interval[2] = {0.0, 41.0};

    // test compression and decompression

    // for each longitude
    for (double lon = lon_interval[0]; lon <= lon_interval[1]; lon += 0.001)
    {

        // pick a random latitude in the interval lat_interval
        const double lat = lat_interval[0] + (rand() % (int)((lat_interval[1] - lat_interval[0]) * 1000)) / 1000.0;
        // pick a random speed in the interval speed_interval
        const double speed = speed_interval[0] + (rand() % (int)((speed_interval[1] - speed_interval[0]) * 100)) / 100.0;

        // print lat and long
        printf("lon: %f | lat: %f | speed: %f", lon, lat, speed);

        // create a compressed string
        char *compressed_gps = compressGPS(lat, lon);
        char *compressed_speed = compressSpeed(speed);

        //concate
        char *concatenated = concatenate(compressed_gps, compressed_speed);

        /////////////////////////////////////
        // simulate satellite transmission //
        /////////////////////////////////////

        // expands
        char **expanded = expand(concatenated);

        char *expanded_gps = expanded[0];
        char *expanded_speed = expanded[1];

        double *decompressed_gps = decompressGPS(expanded_gps);
        double decompressed_speed = decompressSpeed(expanded_speed);

        printf(" || lon: %f | lat: %f | speed: %f", decompressed_gps[1], decompressed_gps[0], decompressed_speed);

        // check if the decompressed coordinates are the same as the original coordinates (with accuracy of 1e-3)
        if (fabs(lat - decompressed_gps[0]) > 1e-3 || fabs(lon - decompressed_gps[1]) > 1e-3)
        {
            // if not, throw an error
            printf("Error: decompressed coordinates are not the same as the original coordinates\n");
            // exit the program
            exit(1);
        }
        
        if (speed < 9.92 && speed >= 0)
        {
            if (fabs(decompressed_speed - speed) > 0.015)
            {
                // if not, throw an error
                printf("\nError: decompressed speed is not the same as the original speed");
                // exit the program
                exit(1);
            }
           
        }else if (speed <= 40.92 && speed >= 0)
        {
            if (fabs(decompressed_speed - speed) > 1)
            {
                // if not, throw an error
                printf("\nError: decompressed speed is not the same as the original speed");
                // exit the program
                exit(1);
            }
            
        }
        else
        {
            if (decompressed_speed != 0)
            {
                // if not, throw an error
                printf("\nError: decompressed speed should be zero\n");
                // exit the program
                exit(1);
            }
        }
         printf(" | OK\n");
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("time spent : %f\n", time_spent);
}

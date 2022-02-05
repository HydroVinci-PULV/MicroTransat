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

    // test compression and decompression
    
    // for each longitude and latitude
    for (double lon = lon_interval[0]; lon <= lon_interval[1]; lon+=0.001)
    {

        // pick a random latitude in the interval
        const double lat = 10.0;

         // print lat and long
            printf("lat: %f | lon: %f", lat, lon);

            // create a compressed string
            char *compressed = compress(lat, lon);

            // decompress the string
            double *coord = decompress(compressed);

            // check if the decompressed coordinates are the same as the original coordinates (with accuracy of 1e-3)
            if (fabs(lat - coord[0]) > 1e-3 || fabs(lon - coord[1]) > 1e-3)
            {
                // if not, throw an error
                printf("Error: decompressed coordinates are not the same as the original coordinates\n");
                // exit the program
                exit(1);
            }else{
                printf(" | OK\n");
            }
    }

    for (double lat = lat_interval[0]; lat <= lat_interval[1]; lat+=0.001)
        {
            const double lon = -10.0;
            // print lat and long
            printf("lat: %f | lon: %f", lat, lon);

            // create a compressed string
            char *compressed = compress(lat, lon);

            // decompress the string
            double *coord = decompress(compressed);

            // check if the decompressed coordinates are the same as the original coordinates (with accuracy of 1e-3)
            if (fabs(lat - coord[0]) > 1e-3 || fabs(lon - coord[1]) > 1e-3)
            {
                // if not, throw an error
                printf("Error: decompressed coordinates are not the same as the original coordinates\n");
                // exit the program
                exit(1);
            }else{
                printf(" | OK\n");
            }
        }

clock_t end = clock();
double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
printf("time spent : %f\n", time_spent);

}

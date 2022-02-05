// includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "telemetry_utils.h"

#pragma region CONSTANTS

// precision of the latitude and longitude
#define GPS_PRECISION 3

// padding to add to the latitude to make it always positive
#define LAT_PADDING 55000

// padding to add to the longitude to make it always positive
#define LON_PADDING 98000

// mask to extract the last byte of an int32
#define MASK_FULL 0x000000FF

// mask to extract the last bit of an int32
#define MASK_UNO 0x00000001

// mask to extract the last 7 bits of an int32
#define MASK_SEVEN 0x000000FE

// mask to extract the last 2 bits of an int32
#define MASK_DOS 0x00000003

#define SPEED_PRECISION 2

// recognizable pattern to indicate to the decoder that the speed is over 9.92 knots
#define SPEED_PATTERN 0x03E0

#define SPEED_PATTERN_MASK 0x001F

#pragma endregion

#pragma region METHODS

double *decompressGPS(char *compressed);
char *compressGPS(double lat, double lon);
char* compressSpeed(double speed);
double decompressSpeed(char *compressed);
char *concatenate(char *compressed_gps, char *compressed_speed);
char **expand(char *compressed);

#pragma endregion


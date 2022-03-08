#include "telemetry.h"

// uncomment when debugging
// comment when unit testing
// void main()
// {
//     char *compressed_gps = compressGPS(48.897,2.236);
//     char *compressed_speed = compressSpeed(1.23);
//     char *compressed = concatenate(compressed_gps, compressed_speed);
//     expand(compressed);
// }

// function that compress GPS coordinates to a char array
char *compressGPS(double lat, double lon)
{
    // init a byte array 5 bytes long to store the compressed data
    char *compressed = (char *)malloc(5);

    // convert lat and long to ints by multiplying by 10^precision
    int lat_int = (int)(lat * pow(10, GPS_PRECISION));
    int lon_int = (int)(lon * pow(10, GPS_PRECISION));

    // add padding to lat and long
    lat_int += LAT_PADDING;
    lon_int += LON_PADDING;

    if (lat_int < 0 || lon_int < 0)
    {
        // throw an error if lat or long is negative
        printf("Error: lat or long is negative\n");
        // exit the program
        exit(1);
    }

#ifdef DEBUG
    print_int_bits(lat_int);
    print_int_bits(lon_int);

    // print lat and long
    printf("lat: %d\n", lat_int);
    printf("lon: %d\n", lon_int);
#endif

    // each int requires a 17 bit-field
    // so we need to split the int into 3 bytes and combine them into 5 bytes

    compressed[0] = (char)lat_int;
    compressed[1] = (char)(lat_int >> 8);
    compressed[2] = (char)((lat_int >> 16) | (lon_int << 1));
    compressed[3] = (char)(lon_int >> 7);
    compressed[4] = (char)(lon_int >> 15);

#ifdef DEBUG
    print_bits(compressed[0]);
    print_bits(compressed[1]);
    print_bits(compressed[2]);
    print_bits(compressed[3]);
    print_bits(compressed[4]);
#endif

    return compressed;
}

// function that decompress a byte array to an array of doubles
double *decompressGPS(char *compressed)
{
    // init an array of doubles to store the decompressed data
    double *decompressed = (double *)malloc(2 * sizeof(double));

    // retrieve padded lat from the byte array
    uint32_t lat;

#ifdef DEBUG
    uint32_t byte1 = (uint32_t)compressed[0];
    uint32_t masked_byte1 = byte1 & MASK_FULL;

    uint32_t byte2 = (uint32_t)compressed[1];
    uint32_t masked_byte2 = byte2 & MASK_FULL;
    uint32_t masked_moved_byte2 = masked_byte2 << 8;

    uint32_t byte3 = (uint32_t)compressed[2];
    uint32_t masked_byte3 = byte3 & MASK_UNO;
    uint32_t masked_moved_byte3 = masked_byte3 << 16;

    lat = masked_byte1 | masked_moved_byte2 | masked_moved_byte3;

    printf("byte1:              ");
    print_uint32_bits(byte1);
    printf("masked_byte1:       ");
    print_uint32_bits(masked_byte1);

    printf("byte2: ##############################################################\n");
    printf("byte2:              ");
    print_uint32_bits(byte2);
    printf("masked_byte2:       ");
    print_uint32_bits(masked_byte2);
    printf("masked_moved_byte2: ");
    print_uint32_bits(masked_moved_byte2);

    printf("byte3: ##############################################################\n");
    printf("byte3:              ");
    print_uint32_bits(byte3);
    printf("masked_byte3:       ");
    print_uint32_bits(masked_byte3);
    printf("masked_moved_byte3: ");
    print_uint32_bits(masked_moved_byte3);

    printf("lat: ");
    print_uint32_bits(lat);
#else
    lat = (((uint32_t)compressed[0]) & MASK_FULL) | (((uint32_t)compressed[1] & MASK_FULL) << 8) | (((uint32_t)compressed[2] & MASK_UNO) << 16);
#endif

    // retrieve padded long from the byte array
    uint32_t lon;
#ifdef DEBUG
    uint32_t byte3_2 = (uint32_t)compressed[2];
    uint32_t masked_byte3_2 = byte3_2 & MASK_SEVEN;
    uint32_t masked_moved_byte3_2 = masked_byte3_2 >> 1;

    uint32_t byte4 = (uint32_t)compressed[3];
    uint32_t masked_byte4 = byte4 & MASK_FULL;
    uint32_t masked_moved_byte4 = masked_byte4 << 7;

    uint32_t byte5 = (uint32_t)compressed[4];
    uint32_t masked_byte5 = byte5 & MASK_DOS;
    uint32_t masked_moved_byte5 = masked_byte5 << 15;

    lon = masked_moved_byte3_2 | masked_moved_byte4 | masked_moved_byte5;

    printf("byte3_2: ##############################################################\n");
    printf("byte3_2:            ");
    print_uint32_bits(byte3_2);
    printf("masked_byte3_2:     ");
    print_uint32_bits(masked_byte3_2);
    printf("masked_moved_byte32:");
    print_uint32_bits(masked_moved_byte3_2);

    printf("byte4: ##############################################################\n");
    printf("byte4:              ");
    print_uint32_bits(byte4);
    printf("masked_byte4:       ");
    print_uint32_bits(masked_byte4);
    printf("masked_moved_byte4: ");
    print_uint32_bits(masked_moved_byte4);

    printf("byte5: ##############################################################\n");
    printf("byte5:              ");
    print_uint32_bits(byte5);
    printf("masked_byte5:       ");
    print_uint32_bits(masked_byte5);
    printf("masked_moved_byte5: ");
    print_uint32_bits(masked_moved_byte5);

    printf("lon: ");
    print_uint32_bits(lon);
#else
    lon = ((((uint32_t)compressed[2]) & MASK_SEVEN) >> 1) | ((((uint32_t)compressed[3]) & MASK_FULL) << 7) | ((((uint32_t)compressed[4]) & MASK_DOS) << 15);
#endif

    // remove padding from lat and long
    // /!\ lat and lon can now be negative
    int lat_int = lat - LAT_PADDING;
    int lon_int = lon - LON_PADDING;

    // convert lat and long to double
    decompressed[0] = (double)lat_int / pow(10, GPS_PRECISION);
    decompressed[1] = (double)lon_int / pow(10, GPS_PRECISION);

    return decompressed;
}

// function that compress boat speed to a byte array
char *compressSpeed(double speed)
{

#ifdef DEBUG
    printf("speed: %f\n", speed);
#endif

    // if speed is negative, set it to 0
    if (speed < 0)
    {
#ifdef DEBUG
        printf("speed negative\n");
#endif
        speed = 0;
    }

    // init a byte array 2 bytes long to store the compressed data
    char *compressed = (char *)malloc(2);

    if (speed < 9.92)
    {
        // convert speed to int
        uint16_t speed_int = speed * pow(10, SPEED_PRECISION);

#ifdef DEBUG
        printf("speed_int: ");
        print_uint16_bits(speed_int);
#endif

        compressed[0] = (char)speed_int;
        compressed[1] = (char)(speed_int >> 8);
    }
    else if (speed <= 40.92) // max speed transmissible with this method, safe enough
    {
        // convert speed to int without the decimal part
        uint16_t speed_int = (uint16_t)(speed - 9.92);

#ifdef DEBUG
        printf("speed_int: ");
        print_uint16_bits(speed_int);
#endif
        // add the pattern to the speed
        speed_int = speed_int | SPEED_PATTERN;

#ifdef DEBUG
        printf("speed_int: ");
        print_uint16_bits(speed_int);
#endif

        compressed[0] = (char)speed_int;
        compressed[1] = (char)(speed_int >> 8);
    }
    else
    {
        compressed[0] = 0;
        compressed[1] = 0;
    }

#ifdef DEBUG
    printf("compressed[0]: ");
    print_bits(compressed[0]);
    printf("compressed[1]: ");
    print_bits(compressed[1]);
#endif

    return compressed;
}

// function that decompress boat speed from a byte array
double decompressSpeed(char *compressed)
{
    double speed;

    // compressed is a byte array 2 bytes long, convert it to an uint16_t
    uint16_t speed_int = ((uint16_t)compressed[0] & MASK_FULL) | (((uint16_t)compressed[1] & MASK_FULL) << 8);

#ifdef DEBUG
    printf("speed_int: ");
    print_uint16_bits(speed_int);
#endif

    // check if SPEED_PATTERN is present
    if ((speed_int & SPEED_PATTERN) == SPEED_PATTERN)
    {
        // remove the pattern from the speed_int
        speed_int &= SPEED_PATTERN_MASK;

#ifdef DEBUG
        printf("speed_int: ");
        print_uint16_bits(speed_int);
#endif

        speed_int += 10;

        speed = (double)speed_int;
    }
    else
    {
        speed = (double)speed_int / pow(10, SPEED_PRECISION);
    }

    return speed;
}

char *compressHeading(double heading)
{
    // init a byte array 2 bytes long to store the compressed data
    char *compressed = (char *)malloc(2);

    // convert heading to int without the decimal part
    uint16_t heading_int = (uint16_t)heading;

    compressed[0] = (char)heading_int;
    compressed[1] = (char)(heading_int >> 8);

    return compressed;
}

double decompressHeading(char *compressed)
{
    double heading = 
}

// function that concatenate byte arrays from different compressors
char *concatenate(char *compressed_gps, char *compressed_speed)
{
    // init a byte array to store the concatenated data
    char *compressed = (char *)malloc(6);

    // concatenate the compressed data
    compressed[0] = compressed_gps[0];
    compressed[1] = compressed_gps[1];
    compressed[2] = compressed_gps[2];
    compressed[3] = compressed_gps[3];
    compressed[4] = (compressed_gps[4] & MASK_DOS) | ((compressed_speed[0] << 2) & 0xFC);
    compressed[5] = ((compressed_speed[0] >> 6) & MASK_DOS) | ((compressed_speed[1] << 2) & 0xC);
#ifdef DEBUG
    printf("concatenate: \n");
    print_bits(compressed[0]);
    print_bits(compressed[1]);
    print_bits(compressed[2]);
    print_bits(compressed[3]);
    print_bits(compressed[4]);
    print_bits(compressed[5]);
#endif

    return compressed;
}

// function that expands a concatenated byte array to an array of char arrays
char **expand(char *concatenated)
{
    // init an array of char arrays to store the expanded data
    char **expanded = (char **)malloc(2 * sizeof(char *));

    char *compressed_gps = (char *)malloc(5);
    char *compressed_speed = (char *)malloc(2);

    // copy the compressed data to the char arrays
    compressed_gps[0] = concatenated[0];
    compressed_gps[1] = concatenated[1];
    compressed_gps[2] = concatenated[2];
    compressed_gps[3] = concatenated[3];
    compressed_gps[4] = concatenated[4] & MASK_DOS;

    compressed_speed[0] = ((concatenated[4] >> 2) & 0x3F) | ((concatenated[5] << 6) & 0xC0);
    compressed_speed[1] = (concatenated[5] >> 2) & MASK_DOS;

#ifdef DEBUG
    printf("gps: \n");
    print_bits(compressed_gps[0]);
    print_bits(compressed_gps[1]);
    print_bits(compressed_gps[2]);
    print_bits(compressed_gps[3]);
    print_bits(compressed_gps[4]);

    printf("speed: \n");
    print_bits(compressed_speed[0]);
    print_bits(compressed_speed[1]);
#endif

    expanded[0] = compressed_gps;
    expanded[1] = compressed_speed;

    return expanded;
}
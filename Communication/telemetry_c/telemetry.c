#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

typedef unsigned int uint;

void print_bits(char c);
void print_int_bits(int i);
void print_int32_bits(int i);
void print_uint8_bits(uint8_t i);
void print_uint16_bits(uint16_t i);
void print_uint32_bits(uint32_t i);
double* decompress(char* compressed);
char* compress(double lat, double lon);

void main() {

    double* coord = decompress(compress(47.419, -6.380));
    printf("%f %f\n", coord[0], coord[1]);
}

const int precision = 3;
const int lat_padding = 55000;
const int long_padding = 98000;
const uint32_t mask = 0x000000FF;
const uint32_t mask_uno = 0x00000001;

// function that compress a string to a byte array
char* compress(double lat, double lon) {
    // init a byte array 5 bytes long to store the compressed data
    char* compressed = (char*) malloc(5);

    // convert lat and long to ints by multiplying by 10^precision
    int lat_int = (int) (lat * pow(10, precision));
    int lon_int = (int) (lon * pow(10, precision));

    // add padding to lat and long
    lat_int += lat_padding;
    lon_int += long_padding;

    if (lat_int < 0 || lon_int < 0) {
        // throw an error if lat or long is negative
        printf("Error: lat or long is negative\n");
        // exit the program
        exit(1);  
    }

    print_int_bits(lat_int);
    print_int_bits(lon_int);

    // print lat and long
    printf("lat: %d\n", lat_int);
    printf("lon: %d\n", lon_int);

    // each int requires a 17 bit-field
    // so we need to split the int into 3 bytes and combine them into 5 bytes

    compressed[0] = (char)lat_int;
    compressed[1] = (char)(lat_int >> 8);
    compressed[2] = (char)((lat_int >> 16) | (lon_int << 1));
    compressed[3] = (char)(lon_int >> 7);
    compressed[4] = (char)(lon_int >> 15);

    print_bits(compressed[0]);
    print_bits(compressed[1]);
    print_bits(compressed[2]);
    print_bits(compressed[3]);
    print_bits(compressed[4]);

    return compressed;
}

// function that decompress a byte array to an array of doubles
double* decompress(char* compressed) {
    // init an array of doubles to store the decompressed data
    double* decompressed = (double*) malloc(2 * sizeof(double));

    uint32_t lat_uint = 0;
    uint32_t byte1 = (uint32_t) compressed[0];
    printf("byte1:");
    print_uint32_bits(byte1);
    uint32_t byte2 = (uint32_t) compressed[1];
    uint32_t masked_byte1 = byte1 & mask;
    printf("byte2: ###############################\n");
    printf("byte2:              ");
    print_uint32_bits(byte2);
    uint32_t masked_byte2 = byte2 & mask;
    printf("masked_byte2:       ");
    print_uint32_bits(masked_byte2);
    uint32_t masked_moved_byte2 = (byte2 & mask) << 8;
    printf("masked_moved_byte2: ");
    print_uint32_bits(masked_moved_byte2);

    printf("byte3: ###############################\n");
    printf("byte3:              ");
    uint32_t byte3 = (uint32_t) compressed[2];
    print_uint32_bits(byte3);
    uint32_t masked_byte3 = byte3 & mask_uno;
    printf("masked_byte3:       ");
    print_uint32_bits(masked_byte3);
    uint32_t masked_moved_byte3 = (byte3 & mask_uno) << 16;
    printf("masked_moved_byte3: ");
    print_uint32_bits(masked_moved_byte3);

    uint32_t lat = masked_byte1 | masked_moved_byte2 | masked_moved_byte3;
    printf("lat: ");
    print_uint32_bits(lat);
        

    

    return decompressed;
}

// function that prints a char as bits string
void print_bits(char c) {
    int i;
    for (i = 7; i >= 0; i--) {
        printf("%d", (c >> i) & 1);
    }
    printf("\n");
}

// function that prints a int as bits string
void print_int_bits(int i) {
    int j;
    for (j = 31; j >= 0; j--) {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_int32_bits(int32_t i) {
    int j;
    for (j = 31; j >= 0; j--) {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_uint8_bits(uint8_t i) {
    int j;
    for (j = 7; j >= 0; j--) {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_uint16_bits(uint16_t i) {
    int j;
    for (j = 15; j >= 0; j--) {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_uint32_bits(uint32_t i) {
    int j;
    for (j = 31; j >= 0; j--) {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

// char* decompress(char* compressed) {
//     // init a string to store the decompressed data
//     char* decompressed = (char*) malloc(20);

//     // get the lat and long from the byte array
//     int lat_int = (int) compressed[0] | (int) compressed[1] << 8 | (int) compressed[2] << 16;
//     int lon_int = (int) compressed[3] | (int) compressed[4] << 8;

//     // remove padding from lat and long
//     lat_int -= lat_padding;
//     lon_int -= long_padding;

//     // convert the ints to doubles by dividing by 10^precision
//     double lat = lat_int / pow(10, precision);
//     double lon = lon_int / pow(10, precision);

//     // print the lat and long
//     printf("lat: %f\n", lat);
//     printf("lon: %f\n", lon);

//     return decompressed;
// }
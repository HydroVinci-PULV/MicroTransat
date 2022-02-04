// Telemetry.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <bitset>

using namespace std;

char* compress(double lat, double lon);
double* decompress(char* bin);


int main()
{
   decompress(compress(48.897, 2.236));
}

const int precision = 3;
const int lat_padding = 55000;
const int long_padding = 98000;

// function that compress a string to a byte array
char* compress(double lat, double lon) {
    // create a byte array to hold the compressed data
    char* compressed = new char[5];

    // convert lat and long to ints by multiplying by 10^precision
    int lat_int = (int) (lat * 1000);
    int lon_int = (int) (lon * 1000);

    // add padding to lat and long
    lat_int += lat_padding;
    lon_int += long_padding;

    if (lat_int < 0 || lon_int < 0) {
        // throw an error if lat or long is negative
        throw "lat or long is negative";
    }

    // each int requires a 17 bit-field
    // so we need to split the int into 3 bytes and combine them into 5 bytes

    compressed[0] = (char)lat_int;
    compressed[1] = (char)(lat_int >> 8);
    compressed[2] = (char)((lat_int >> 16) | (lon_int << 1));
    compressed[3] = (char)(lon_int >> 7);
    compressed[4] = (char)(lon_int >> 15);


    return compressed;
}

double* decompress(char* bin) {
    // check bin size
    if (sizeof(bin) != 5) {
        throw "bin must be 5 long";
    }


    bitset<32> test_1 = bitset<32>(bin[0]);
    bitset<32> test_2 = bitset<32>(bin[0] | (bin[1] << 8));

    double* decompressed = new double[2];

    bitset<8> bin_1 = bitset<8>(bin[0]);
    bitset<8> bin_2 = bitset<8>(bin[1]);
    bitset<8> bin_3 = bitset<8>(bin[2]);
    bitset<8> bin_4 = bitset<8>(bin[3]);
    bitset<8> bin_5 = bitset<8>(bin[4]);

    bitset<17> bin_lat = bitset<17>();

    for (int i = 0; i < 17; i++) {
        if (i < 8) {
            bin_lat[i] = bin_1[i];
        }
        else if (i < 16) {
            bin_lat[i] = bin_2[i - 8];
        }
        else {
            bin_lat[i] = bin_3[0];
        }
    }

    int lat_int = bin_lat.to_ulong();
    lat_int -= lat_padding;

    double lat = (double)lat_int / 1000.0;

    return decompressed;
}
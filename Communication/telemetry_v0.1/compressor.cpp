#include <iostream>

using namespace std;

byte* compress(double lat, double lon);

int main(){
    compress(48.858, 2.35);
}


const int precision = 3;
const int lat_padding = 55000;
const int long_padding = 98000;

// function that compress a string to a byte array
byte* compress(double lat, double lon) {
    // create a byte array to hold the compressed data
    byte* compressed = new byte[5];

    // convert lat and long to ints by multiplying by 10^precision
    int lat_int = lat * 1000;
    int lon_int = lon * 1000;

    // add padding to lat and long
    lat_int += lat_padding;
    lon_int += long_padding;

    if(lat_int < 0 || lon_int < 0) {
        // throw an error if lat or long is negative
        throw "lat or long is negative";
    }

    // each int requires a 17 bit-field
    // so we need to split the int into 3 bytes

    compressed[0] = (byte) (lat_int >> 9);


    return compressed;
    
}


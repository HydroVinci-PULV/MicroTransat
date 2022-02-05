#include "telemetry_utils.h"


// function that prints a char as bits string
void print_bits(char c)
{
    int i;
    for (i = 7; i >= 0; i--)
    {
        printf("%d", (c >> i) & 1);
    }
    printf("\n");
}

// function that prints a int as bits string
void print_int_bits(int i)
{
    int j;
    for (j = 31; j >= 0; j--)
    {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

// function that prints a char array as bits string
void print_bits_array(char *c)
{
    int i;
    for (i = 0; i < sizeof c; i++)
    {
        print_bits(c[i]);
    }
}

void print_int32_bits(int32_t i)
{
    int j;
    for (j = 31; j >= 0; j--)
    {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_uint8_bits(uint8_t i)
{
    int j;
    for (j = 7; j >= 0; j--)
    {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_uint16_bits(uint16_t i)
{
    int j;
    for (j = 15; j >= 0; j--)
    {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

void print_uint32_bits(uint32_t i)
{
    int j;
    for (j = 31; j >= 0; j--)
    {
        printf("%d", (i >> j) & 1);
    }
    printf("\n");
}

#pragma endregion
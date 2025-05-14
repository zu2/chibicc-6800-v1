#include <stdio.h>
#include <math.h>

float atanf(float x) {
    float r = 0.0f;
    float x2 = x*x;

    r = (121.0f * x2) / (23.0f + r);  // 11
    r = (100.0f * x2) / (21.0f + r);  // 10
    r = ( 81.0f * x2) / (19.0f + r);  //  9
    r = ( 64.0f * x2) / (17.0f + r);  //  8
    r = ( 49.0f * x2) / (15.0f + r);  //  7
    r = ( 36.0f * x2) / (13.0f + r);  //  6
    r = ( 25.0f * x2) / (11.0f + r);  //  5
    r = ( 16.0f * x2) / ( 9.0f + r);  //  4
    r = (  9.0f * x2) / ( 7.0f + r);  //  3
    r = (  4.0f * x2) / ( 5.0f + r);  //  2
    r = (         x2) / ( 3.0f + r);  //  1

    return (x / (1.0f + r)); // 0
}

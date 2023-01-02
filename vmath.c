#include "vmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

vec3 v3init(float x, float y, float z) {
    vec3 new;
    new.x = x;
    new.y = y;
    new.z = z;
    return new;
}

float v3dot(vec3 a, vec3 b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}
vec3 v3cross(vec3 a, vec3 b) {
    vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

vec3 v3mul(vec3 a, float b) {
    return v3init(a.x * b, a.y * b, a.z * b);
}
vec3 v3div(vec3 a, float b) {
    float f = 1.0/b;
    return v3init(a.x * f, a.y * f, a.z * f);
}

vec3 v3add(vec3 a, vec3 b) {
    return v3init(a.x + b.x, a.y + b.y, a.z + b.z);
}
vec3 v3sub(vec3 a, vec3 b) {
    return v3init(a.x - b.x, a.y - b.y, a.z - b.z);
}


vec3 v3neg(vec3 input) {
    return v3init(-input.x, -input.y, -input.z);
}

float v3magn(vec3 input) {
    return sqrtf(v3dot(input, input));
}


vec3 v3unit(vec3 input) {
    float factor = 1.0/v3magn(input);
    return v3mul(input, factor);
}

vec3 m3v3mul(vec3 a[3], vec3 b) {
    vec3 ra = v3mul(a[0], b.x);
    vec3 rb = v3mul(a[1], b.y);
    vec3 rc = v3mul(a[2], b.z);
    vec3 result = {
        ra.x + rb.x + rc.x,
        ra.y + rb.y + rc.y,
        ra.z + rb.z + rc.z,
    };
    return result;
}

char * v3fmt(vec3 input) {
    char * str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(%.4g, %.4g, %.4g)", input.x, input.y, input.z);
    return str;
}

#include "vmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    return (vec3) { a.x * b, a.y * b, a.z * b};
}
vec3 v3div(vec3 a, float b) {
    float f = 1.0/b;
    return (vec3) { a.x * f, a.y * f, a.z * f };
}

vec3 v3add(vec3 a, vec3 b) {
    return (vec3) { a.x + b.x, a.y + b.y, a.z + b.z };
}
vec3 v3sub(vec3 a, vec3 b) {
    return (vec3) { a.x - b.x, a.y - b.y, a.z - b.z };
}


vec3 v3neg(vec3 input) {
    return (vec3) { -input.x, -input.y, -input.z};
}

float v3magn(vec3 input) {
    return sqrtf(v3dot(input, input));
}


vec3 v3unit(vec3 input) {
    float factor = 1.0/v3magn(input);
    return v3mul(input, factor);
}

float v4dot(vec4 a, vec4 b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}
vec4 v4cross(vec4 a, vec4 b) {
    vec4 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
    return result;
}


vec3 m3v3mul(matrix3 a, vec3 b) {
    vec3 ri = v3mul(a.i, b.x);
    vec3 rj = v3mul(a.j, b.y);
    vec3 rk = v3mul(a.k, b.z);
    vec3 result = {
        ri.x + rj.x + rk.x,
        ri.y + rj.y + rk.y,
        ri.z + rj.z + rk.z,
    };
    return result;
}


vec4 v4mul(vec4 a, float b) {
    return (vec4) { a.x * b, a.y * b, a.z * b, a.w * b};
}
vec4 v4div(vec4 a, float b) {
    float f = 1.0/b;
    return (vec4) { a.x * f, a.y * f, a.z * f, a.w * f};
}
vec4 v4add(vec4 a, vec4 b) {
    return (vec4) { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
vec4 v4sub(vec4 a, vec4 b) {
    return (vec4) { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

vec4 m4v4mul(matrix4 a, vec4 b) {
    vec4 ri = v4mul(a.i, b.x);
    vec4 rj = v4mul(a.j, b.y);
    vec4 rk = v4mul(a.k, b.z);
    vec4 rt = v4mul(a.t, b.w);
    vec4 result = {
        ri.x + rj.x + rk.x + rt.x,
        ri.y + rj.y + rk.y + rt.y,
        ri.z + rj.z + rk.z + rt.z,
        ri.w + rj.w + rk.w + rt.w
    };
    return result;
}

matrix4 m4mul(matrix4 b, matrix4 a) {
    return (matrix4) {
        m4v4mul(b,a.i),
        m4v4mul(b,a.j),
        m4v4mul(b,a.k),
        m4v4mul(b,a.t),
    };
}

vec3 hgtocar(vec4 ip) {
    return v3div((vec3) {ip.x, ip.y, ip.z}, ip.w);
}


//these functions all cause memory leaks, but they're just for debugging.
char * v3fmt(vec3 input) {
    char * str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(%.4g, %.4g, %.4g)", input.x, input.y, input.z);
    return str;
}
char * v4fmt(vec4 input) {
    char * str = malloc(100 * sizeof(char));
    snprintf(str, 100, "(%.4g, %.4g, %.4g, %.4g)", input.x, input.y, input.z, input.w);
    return str;
}
char * m4fmt(matrix4 input) {
    char * str = malloc(300 * sizeof(char));
    snprintf(str, 300, "\n %.4g, %.4g, %.4g, %.4g \n %.4g, %.4g, %.4g, %.4g \n %.4g, %.4g, %.4g, %.4g \n %.4g, %.4g, %.4g, %.4g", input.i.x, input.j.x, input.k.x, input.t.x, input.i.y, input.j.y, input.k.y, input.t.y, input.i.z, input.j.z, input.k.z, input.t.z, input.i.w, input.j.w, input.k.w, input.t.w);
    return str;
}

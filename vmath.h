typedef struct {
    float x;
    float y;
    float z;
} vec3;

char * v3fmt(vec3 input);
vec3 v3init(float x, float y, float z);
float v3dot(vec3 a, vec3 b);
vec3 v3cross(vec3 a, vec3 b);
vec3 v3mul(vec3 a, float b);
vec3 v3div(vec3 a, float b);
vec3 v3add(vec3 a, vec3 b);
vec3 v3sub(vec3 a, vec3 b);
float v3magn(vec3 input);
vec3 v3unit(vec3 input);
vec3 v3neg(vec3 input);
vec3 m3v3mul(vec3 a[3], vec3 b);

#define V3TO4(INPT,W) ( (vec4) { INPT.x, INPT.y, INPT.z, W })
#define V4TO3(INPT) ( (vec3) { INPT.x, INPT.y, INPT.z })


typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    vec3 i;
    vec3 j;
    vec3 k;
} matrix3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    vec4 i;
    vec4 j;
    vec4 k;
    vec4 t;
} matrix4;

char * v3fmt(vec3 input);

float v3dot(vec3 a, vec3 b);
vec3 v3cross(vec3 a, vec3 b);
vec3 v3mul(vec3 a, float b);
vec3 v3div(vec3 a, float b);
vec3 v3add(vec3 a, vec3 b);
vec3 v3sub(vec3 a, vec3 b);
float v3magn(vec3 input);
vec3 v3unit(vec3 input);
vec3 v3neg(vec3 input);
vec3 m3v3mul(matrix3 a, vec3 b);


char * v4fmt(vec4 input);
char * m4fmt(matrix4 input);

float v4dot(vec4 a, vec4 b);
vec4 v4cross(vec4 a, vec4 b);
vec4 v4mul(vec4 a, float b);
vec4 v4div(vec4 a, float b);
vec4 v4add(vec4 a, vec4 b);
vec4 v4sub(vec4 a, vec4 b);
vec4 m4v4mul(matrix4 a, vec4 b);
matrix4 m4mul(matrix4 b, matrix4 a);

vec3 hgtocar(vec4 ip);

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef long int lint;
typedef signed char sbyte;
// typedef long int entity;
// typedef uint32_t uint;
// typedef uint64_t ulong;

// Used for graphics
#ifdef zox_sdl
    typedef GLint gint;
    typedef GLuint guint;
    typedef GLchar gchar;
    typedef GLsizeiptr gsize;   // buffer sizes
    typedef GLsizei gsizei;   // regular sizes
#else
    typedef int gint;
    typedef uint guint;
    typedef char gchar;
    typedef uint gsize;
    typedef uint gsizei;
#endif

typedef void (*event_pointer)(void*);

typedef struct {
    gint x, y;
} gint2;

typedef struct {
    guint x, y;
} guint2;

const guint2 guint2_zero = (guint2) { 0, 0 };

// base type, watch out for name conflicts tho

typedef struct {
    sbyte x, y;
} sbyte2;

typedef struct {
    sbyte x, y, z;
} sbyte3;

typedef struct {
    ushort x, y;
} ushort2;

typedef struct {
    ushort x, y, z;
} ushort3;

typedef struct {
    byte x, y;
} byte2;

typedef struct {
    byte x, y, z;
} byte3;

typedef struct {
    byte x, y, z, w;
} byte4;

typedef struct {
    int x, y;
} int2;

typedef struct {
    int x, y, z;
} int3;

typedef struct {
    int x, y, z, w;
} int4;

typedef struct {
    unsigned int x, y;
} uint2;

typedef struct {
    unsigned int x, y, z;
} uint3;

typedef struct {
    float x, y;
} float2;

typedef struct {
    float x, y, z;
} float3;

typedef struct {
    float x, y, z, w;
} float4;

typedef struct {
    float x, y, z, w, u, v;
} float6;

typedef struct {
    float3 x, y, z;
} float3x3;

typedef struct {
    float4 x, y, z, w;
} float4x4;

typedef struct {
    double x, y;
} double2;

typedef struct {
    double x, y, z;
} double3;

typedef struct {
    double x, y, z, w;
} double4;

typedef struct {
    byte r, g, b, a;
} color;

typedef struct {
    byte r, g, b;
} color_rgb;

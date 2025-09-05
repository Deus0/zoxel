#define zox_light_type_beam_start 0
#define zox_light_type_beam 1
#define zox_light_type_flood 2

typedef struct {
    void* ptr;
    byte value;
    byte type;  // for now until i decouple it..
} LightNode;

// beams queued
typedef struct {
    // 0 light - 1 dark
    byte type;
    byte light;
    byte3 pos;
    byte depth;
    byte distance;
} LightUpdate;

typedef struct {
    // 0 Propogate - 1 Beam
    byte type;
    byte3 pos;
    byte depth;
    byte light;
    byte distance;
} DarkUpdate;
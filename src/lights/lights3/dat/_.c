#define zox_light_type_beam_start 0
#define zox_light_type_beam 1
#define zox_light_type_flood 2
#define zox_light_type_dim_start 3     // water placed at pos: its beam relit dimmed
#define zox_light_type_dim 4           // continued from the chunk above, relit dimmed

// two lights a voxel, never mixed (doc/plans/day_night.md): the sky's (sunbeams and their floods: LightOctree), and
//  blocks' (glow stones, lamps: BlockLightOctree); updates carry their channel, so one queue a chunk holds both
#define zox_light_sky 0
#define zox_light_block 1

typedef struct {
    void* ptr;
    byte value;
} LightOctree;
typedef LightOctree BlockLightOctree;   // the same tree, its own component

// beams queued
typedef struct {
    byte light;
    byte3 pos;
    byte depth;
} SunlightUpdate;

// beams queued
typedef struct {
    // 0 light - 1 dark
    // byte type;
    byte light;
    byte3 pos;
    byte depth;
    byte distance;
    byte channel;   // zox_light_sky or zox_light_block
} LightUpdate;

typedef struct {
    // 0 Propogate - 1 Beam
    byte type;
    byte3 pos;
    byte depth;
    byte light;
    byte distance;
    byte channel;   // zox_light_sky or zox_light_block
} DarkUpdate;

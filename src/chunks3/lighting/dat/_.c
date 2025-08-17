typedef struct {
    void* ptr;
    byte value;
    byte type;  // for now until i decouple it..
} LightNode;

typedef struct {
    // 0 light - 1 dark
    byte type;
    byte light;
    byte3 pos;
    byte depth;
    byte distance;
} PropogateUpdate;

// beams queued
typedef struct {
    // 0 light - 1 dark
    byte type;
    byte light;
    byte3 pos;
} SunlightUpdate;


typedef struct {
    // 0 Propogate - 1 Beam
    byte type;
    byte3 pos;
    byte depth;
    byte light;
    byte distance;
} DarkUpdate;
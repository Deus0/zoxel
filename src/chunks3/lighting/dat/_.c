typedef struct {
    void* ptr;
    byte value;
    byte type;  // for now until i decouple it..
} LightNode;

typedef struct {
    byte value;
    byte3 positionl;
} PropogateUpdate;

// beams queued
typedef struct {
    byte3 pos;
    byte light;
} SunlightUpdate;
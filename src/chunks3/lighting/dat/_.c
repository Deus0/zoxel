typedef struct {
    void* ptr;
    byte value;
    byte type;  // for now until i decouple it..
} LightNode;

typedef struct {
    byte value;
    byte3 positionl;
} LightNodeUpdate;

// beams queued
typedef struct {
    byte2 positionl;
} SunlightUpdate;
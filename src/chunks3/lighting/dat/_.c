typedef struct {
    void* ptr;
    byte value;
    byte type;  // for now until i decouple it..
} LightNode;

typedef struct {
    byte3 pos;
    byte depth;
    byte distance;
    byte type;  // 0 flood - 1 reverse
    byte light;
} PropogateUpdate;

// beams queued
typedef struct {
    byte3 pos;
    byte light;
} SunlightUpdate;
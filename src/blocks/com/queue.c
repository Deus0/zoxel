// NOTE: Assume Max Depth
typedef struct {
    byte3 position;
    float damage;
} BlockDamageUpdate;
zoxc_queue(BlockDamageQueue, BlockDamageUpdate, 1);

typedef struct {
    void (*value)(ecs*, entity, byte, byte);
} zox_game_event;

zoxel_dynamic_array(zox_game_event)
// should keep function pointers
// create_array_d_has(int)

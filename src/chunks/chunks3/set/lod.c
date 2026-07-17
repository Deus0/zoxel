// TODO: Move to streaming modules
byte vox_lod_near = 1;
byte terrain_lod_near = 1;      // also character far
byte terrain_lod_far = 8;       // 1 | 2 | 4 | 8 | 16
byte render_distance_y = 4;     // vertical
byte disable_terrain_lods = 0;
byte terrain_lod_near_min = 1;
byte terrain_lod_near_max = 16;
const byte terrain_lod_far_min = 1;
const byte terrain_lod_far_max = 48;  // options, min + percentage*max
byte terrain_lod_far_buffer = 2;    // buffer the near to far - adds a minimum low lods
const byte render_distance_y_min = 1;
// const byte render_distance_y_max = 12;
#define render_distance_y_max 8
float settings_terrain_lod_start = 0.25f;
extern int get_characters_count(ecs *world);
extern int count_entities_npc(ecs *world);

uint debug_label_lods(ecs *world, entity player, char *buffer, uint size, uint index) {
    (void) player;
    index += snprintf(buffer + index, size - index, "### Lod Settings ###\n");
    index += snprintf(buffer + index, size - index, " - vox near [%i]\n", vox_lod_near);
    index += snprintf(buffer + index, size - index, " - terrain near [%i]\n", terrain_lod_near);
    index += snprintf(buffer + index, size - index, " - terrain far [%i]]\n", terrain_lod_far);
    index += snprintf(buffer + index, size - index, " - terrain vertical [%i]\n", render_distance_y);
    index += snprintf(buffer + index, size - index, " - npcs [%i]\n", count_entities_npc(world));
    return index;
}

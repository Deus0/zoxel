// tst: home_door_glow, a town home lit by its glow stone stays lit after its door is opened and shut (play test
//  2026-11-27: a house's glow stone didn't light it once its door was shut, vulkan, gpu meshing)
//  - the player's game; each home near the player: its glow stone (in its roof), the column of air under it, and its
//    door (a door's base within 8 blocks round its middle, under its roof)
//  - the column's light before; every door opened (door_toggle), then shut: the column's light again, each voxel no
//    darker than before. The shut's dark took the glow's light and nothing lit it back: a dark reaching the stone's
//    side on its last step (often crossing a chunk's edge) stopped without looking round it (lights3/sys/dark.c), it
//    failed ~1 run in 3; tst dark_last_step checks that step, pure
//  - a tool, slow (a whole game): left out of tst.sh all, run it by hand
//  - run: bash bsh/tst.sh home_door_glow
#define zox_tst_hdoor_homes 16
#define zox_tst_hdoor_column 6
typedef struct {
    int3 glow;
    int3 door;
    byte before[zox_tst_hdoor_column];
    byte open[zox_tst_hdoor_column];
} zox_tst_hdoor_home;
zox_tst_hdoor_home zox_tst_hdoor[zox_tst_hdoor_homes];
int zox_tst_hdoor_count = 0;
byte zox_tst_hdoor_stage = 0;
double zox_tst_hdoor_time = 0;

static inline byte zox_tst_hdoor_light(ecs* world, entity terrain, int3 block) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, LightOctree)) {
        return 0;
    }
    const byte3 local = { block.x - chunk_position.x * length, block.y - chunk_position.y * length,
        block.z - chunk_position.z * length };
    return getv_LightOctree(zox_get(chunk, LightOctree), depth, local);
}

static inline void zox_tst_hdoor_read(ecs* world, entity terrain, zox_tst_hdoor_home* home, byte* lights) {
    for (int i = 0; i < zox_tst_hdoor_column; i++) {
        lights[i] = zox_tst_hdoor_light(world, terrain, (int3) { home->glow.x, home->glow.y - 1 - i, home->glow.z });
    }
}

// the homes with a glow stone and a door
static void zox_tst_hdoor_find(ecs* world, entity terrain, entity realm) {
    const int glow_id = zox_getv(zox_get_child_by_id(world, realm, zox_id(BlockGlowStone)), BlockIndex);
    const int door_id = zox_getv(zox_get_child_by_id(world, realm, zox_id(BlockDoor)), BlockIndex);
    const entity region = zox_tst_player_region(world);
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    for (uint t = 0; t < towns_count; t++) {
        entity homes[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, towns[t], homes, zox_children_capacity, zox_id(Home));
        for (uint h = 0; h < count && zox_tst_hdoor_count < zox_tst_hdoor_homes; h++) {
            const int2 c = zox_getv(homes[h], BlockPosition2);
            int glow_y = -999;
            for (int y = -64; y < 128 && glow_y == -999; y++) {
                if (terrain_voxel_at(world, terrain, (int3) { c.x, y, c.y }) == glow_id) {
                    glow_y = y;
                }
            }
            if (glow_y == -999) {
                continue;
            }
            byte found = 0;
            int3 door = int3_zero;
            for (int y = glow_y - 12; y < glow_y && !found; y++)
            for (int x = c.x - 8; x <= c.x + 8 && !found; x++)
            for (int z = c.y - 8; z <= c.y + 8 && !found; z++) {
                if (terrain_voxel_at(world, terrain, (int3) { x, y, z }) == door_id) {
                    door = (int3) { x, y, z };
                    found = 1;
                }
            }
            if (!found) {
                continue;
            }
            zox_tst_hdoor_home* home = &zox_tst_hdoor[zox_tst_hdoor_count++];
            home->glow = (int3) { c.x, glow_y, c.y };
            home->door = door;
            zox_tst_hdoor_read(world, terrain, home, home->before);
        }
    }
}

byte zox_tst_home_door_glow_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    if (!zox_tst_hdoor_time) {
        zox_tst_hdoor_time = zox_current_time;
    }
    const char* waits[3] = { "lights to flood", "the doors open", "the doors shut" };
    zox_tst_waiting = waits[zox_tst_hdoor_stage < 3 ? zox_tst_hdoor_stage : 2];
    if (zox_current_time - zox_tst_hdoor_time < 3) {
        return 0;
    }
    zox_tst_hdoor_time = zox_current_time;
    if (zox_tst_hdoor_stage == 0) {
        zox_tst_hdoor_find(world, terrain, realm);
    }
    if (zox_tst_hdoor_stage == 1) {
        for (int i = 0; i < zox_tst_hdoor_count; i++) {
            zox_tst_hdoor_read(world, terrain, &zox_tst_hdoor[i], zox_tst_hdoor[i].open);
        }
    }
    if (zox_tst_hdoor_stage == 2) {
        return 1;
    }
    for (int i = 0; i < zox_tst_hdoor_count; i++) {
        door_toggle(world, terrain, realm, zox_tst_hdoor[i].door);
    }
    zox_tst_hdoor_stage++;
    return 0;
}

void zox_tst_home_door_glow_check(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    zox_tst_check(zox_tst_hdoor_count, "no homes with a glow stone and a door");
    int darker = 0;
    for (int i = 0; i < zox_tst_hdoor_count; i++) {
        zox_tst_hdoor_home* home = &zox_tst_hdoor[i];
        byte after[zox_tst_hdoor_column];
        zox_tst_hdoor_read(world, terrain, home, after);
        char row[256] = { 0 };
        byte home_darker = 0;
        for (int j = 0; j < zox_tst_hdoor_column; j++) {
            snprintf(row + strlen(row), sizeof(row) - strlen(row), " %i>%i>%i", home->before[j], home->open[j], after[j]);
            home_darker |= after[j] + 4 < home->before[j];
        }
        zox_log("[tst] home_door_glow: home [%i] glow [%i %i %i] door [%i %i %i], under its glow:%s", i,
            home->glow.x, home->glow.y, home->glow.z, home->door.x, home->door.y, home->door.z, row);
        darker += home_darker;
    }
    zox_tst_check(!darker, "[%i] of [%i] homes darker under their glow after their door opened and shut", darker,
        zox_tst_hdoor_count);
}

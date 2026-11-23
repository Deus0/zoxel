#include "void_player.c"
#include "wheel.c"
#include "split.c"
#include "punch.c"
#include "tooltip_flick.c"
extern char zox_screenshot_request[256];
#include "world_only.c"
#include "music.c"
#include "underwater.c"
#include "crouch.c"
#include "walk_bob.c"
#include "auto_jump.c"
// tst: player_spawn, a new realm spawns the player's character standing in the terrain
//  - waits for the player's character, then lets it settle for a few seconds
//  - the character's block is air (not stuck in the ground) and it hasn't fallen out of the world
//  - it stands on ground: a solid block a few blocks under it
//  - run: bash bsh/tst.sh player_spawn (intermittent bugs: --repeat 5)
const double zox_tst_spawn_settle = 3.0;
double zox_tst_spawn_seen = 0;
double zox_tst_spawn_start = 0;

// the voxel at a world block position, -1 if its chunk isn't loaded
static inline int zox_tst_terrain_voxel(ecs* world, entity terrain, int3 block) {
    return terrain_voxel_at(world, terrain, block);
}

// logs the state of the camera's terrain column, so a stalled spawn shows what it waits on
void zox_tst_spawn_dump_column(ecs* world, entity terrain, int3 column) {
    // near chunks stuck generating, and their tunk's generation state
    int stuck = 0;
    iter it2 = zox_children(world, terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity c = it2.entities[j];
            if (!zox_has(c, Chunk3) || !zox_has(c, GenerateChunk) || !zox_has(c, RenderDistance) || zox_getv(c, RenderDistance) > terrain_lod_near) {
                continue;
            }
            if (stuck++ >= 24) {
                continue;
            }
            const entity t = zox_get_link(world, c, TunkLink);
            const int3 p = zox_getv(c, ChunkPosition);
            zox_log("[tst]   generating [%s] at [%ix%ix%i] state [%i] depth [%i] distance [%i] tunk [%s] generate_tunk [%i] tunk_lod [%i]",
                zox_getn(c), p.x, p.y, p.z, zox_getv(c, GenerateChunk), zox_getv(c, NodeDepth), zox_getv(c, RenderDistance),
                zox_valid(t) ? zox_getn(t) : "none",
                zox_valid(t) && zox_has(t, GenerateTunk) ? zox_getv(t, GenerateTunk) : -1,
                zox_valid(t) && zox_has(t, TunkLod) ? zox_getv(t, TunkLod) : -1);
        }
    }
    zox_log("[tst]   near chunks generating [%i]", stuck);
    const TerrainSpawnQueue* queue = zox_get(terrain, TerrainSpawnQueue);
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const entity tunk = int2_hashmap_get(tunks->value, (int2) { column.x, column.z });
    zox_log("[tst] column [%ix%i] queue [%i] tunks [%i] chunks [%i] tunk [%s] valid [%i] loading [%i]",
        column.x, column.z, (int) queue->count, count_int2_hashmap(tunks->value), count_int3_hashmap(chunks->value),
        zox_valid(tunk) ? zox_getn(tunk) : "none", zox_valid(tunk), zox_has(terrain, Loading));
    // near columns with no tunk, i.e. never queued or dropped from the queue
    for (int dx = -terrain_lod_near; dx <= terrain_lod_near; dx++) {
        for (int dz = -terrain_lod_near; dz <= terrain_lod_near; dz++) {
            const int2 p2 = (int2) { column.x + dx, column.z + dz };
            if (!zox_valid(int2_hashmap_get(tunks->value, p2))) {
                zox_log("[tst]   near column [%ix%i] has no tunk (queued [%i])", p2.x, p2.y, get_TerrainSpawnQueue((TerrainSpawnQueue*) queue, p2) != NULL);
            }
        }
    }
    for (int k = 0; k < (int) queue->count && k < 5; k++) {
        const TerrainSpawnUpdate u = queue->ptr[k];
        zox_log("[tst]   queued [%ix%i] distance [%i] level [%i] tunk [%llu]", u.position.x, u.position.y, u.distance, u.level, (unsigned long long) u.tunk);
    }
    for (int y = render_distance_y; y >= -render_distance_y; y--) {
        const entity chunk = int3_hashmap_get(chunks->value, (int3) { column.x, y, column.z });
        if (!zox_valid(chunk)) {
            zox_log("[tst]   chunk y [%i] missing (hashmap [%llu])", y, (unsigned long long) chunk);
            continue;
        }
        int solid = 0;
        const byte depth = zox_getv(chunk, NodeDepth);
        const int length = octree_size(depth);
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        for (int x = 0; x < length; x++) for (int yy = 0; yy < length; yy++) for (int z = 0; z < length; z++) {
            solid += getv_VoxelNode(voxels, depth, (byte3) { x, yy, z }) != 0;
        }
        zox_log("[tst]   chunk y [%i] [%s] depth [%i] solid [%i/%i] generate [%i]",
            y, zox_getn(chunk), depth, solid, length * length * length,
            zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : -1);
    }
}

static inline entity zox_tst_spawn_character(ecs* world) {
    const entity player = dbg_player;
    if (!zox_valid(player)) {
        return 0;
    }
    const entity character = zox_get_link(world, player, CharacterLink);
    return zox_valid(character) && zox_has(character, Position3D) ? character : 0;
}

entity zox_tst_player_character(ecs* world) {
    return zox_tst_spawn_character(world);
}

byte zox_tst_player_spawn_ready(ecs* world) {
    if (zox_tst_void) {
        return zox_tst_void_player_ready(world);
    }
    if (!zox_tst_spawn_start) {
        zox_tst_spawn_start = zox_current_time;
    }
    zox_tst_waiting = "the player's character to spawn in the terrain";
    if (!zox_tst_spawn_character(world)) {
        // streaming state in the waiting message, so a stall shows where it's stuck
        static char waiting[512];
        const entity camera = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CameraLink) : 0;
        if (zox_valid(camera) && zox_valid(local_terrain) && zox_has(camera, StreamPosition)) {
            const float scale = zox_getv(local_terrain, BlockScale);
            const int length = octree_size(zox_getv(local_terrain, NodeDepth));
            const int3 camera_chunk = real_position_to_chunk_position(zox_getv(camera, Position3D), length, scale);
            const int3 stream = zox_getv(camera, StreamPosition);
            snprintf(waiting, sizeof(waiting),
                "the player's character to spawn (camera chunk [%ix%ix%i] stream [%ix%ix%i] dirty [%i] level [%i] player state [%i] blocked [%s] lod far [%i] y [%i] stream [%s])",
                camera_chunk.x, camera_chunk.y, camera_chunk.z,
                stream.x, stream.y, stream.z,
                zox_getv(camera, StreamDirty),
                zox_getv(camera, StreamerLevel),
                zox_getv(dbg_player, PlayerState),
                zox_player_spawn_blocked, terrain_lod_far, render_distance_y, zox_stream_end_status);
            zox_tst_waiting = waiting;
            // once, when streaming stopped: why the camera's column has no spawn position
            //  - once per test, 15s in (a stall fails the test 20s after its last progress)
            static byte dumped = 0;
            if (!dumped && zox_current_time - zox_tst_spawn_start > 15) {
                dumped = 1;
                zox_tst_spawn_dump_column(world, local_terrain, camera_chunk);
            }
        }
        // progress: chunks streamed in, chunks finishing generation, and near chunks loaded (meshed, lit: with far
        //  land every chunk is in a minute before they're all loaded, it read as a stall, tst aesthetic_town)
        if (zox_valid(local_terrain) && zox_has(local_terrain, ChunkLinks)) {
            const ChunkLinks* chunks = zox_get(local_terrain, ChunkLinks);
            const int count = count_int3_hashmap(chunks->value);
            const int generating = zox_count_types(GenerateChunk);
            zox_tst_progress(count * 100000.0 + generating * 1000.0 + zox_stream_end_loaded);
        }
        return 0;
    }
    if (!zox_tst_spawn_seen) {
        zox_tst_spawn_seen = zox_current_time;
        zox_log("[tst] player character spawned after [%.1fs]", zox_tst_spawn_seen - zox_tst_spawn_start);
    }
    zox_tst_waiting = "the player's character to settle";
    return zox_current_time - zox_tst_spawn_seen >= zox_tst_spawn_settle;
}

void zox_tst_player_spawn_check(ecs* world) {
    // meshes lit older than their sides are rebuilt (build_smooth_lights_system), how many
    zox_log("[tst] [metric] stale meshes rebuilt when lit [%u]", zox_smooth_lights_stale);
    const entity character = zox_tst_spawn_character(world);
    zox_tst_check(character, "player character is gone after settling");
    if (!character) {
        return;
    }
    const entity terrain = local_terrain;
    zox_tst_check(zox_valid(terrain), "no terrain");
    if (!zox_valid(terrain)) {
        return;
    }
    const float3 position = zox_getv(character, Position3D);
    const float scale = zox_getv(terrain, BlockScale);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const float lowest = -(render_distance_y + 1) * length * scale;
    zox_log("[tst] character [%s] at [%.1fx%.1fx%.1f], world bottom [%.1f]",
        zox_getn(character), position.x, position.y, position.z, lowest);
    zox_tst_check(isfinite(position.x) && isfinite(position.y) && isfinite(position.z), "character position isn't finite");
    zox_tst_check(position.y > lowest, "character fell out of the world, y [%.1f] below [%.1f]", position.y, lowest);
    // the block the character stands in is air
    const int3 block = real_position_to_block_position(position, scale);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const entity chunk = int3_hashmap_get(chunks->value, chunk_position);
    zox_tst_check(zox_valid(chunk), "no chunk under the character at [%ix%ix%i]", chunk_position.x, chunk_position.y, chunk_position.z);
    if (!zox_valid(chunk)) {
        return;
    }
    const byte3 local = {
        block.x - chunk_position.x * length,
        block.y - chunk_position.y * length,
        block.z - chunk_position.z * length
    };
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const byte voxel = getv_VoxelNode(voxels, depth, local);
    zox_tst_check(!voxel, "character is inside a solid block [%i] at [%ix%ix%i]", voxel, block.x, block.y, block.z);
    // standing on ground, not floating or mid fall: a solid block close under it
    const int ground_range = 4;
    int ground = 0;
    for (int i = 1; i <= ground_range && !ground; i++) {
        const int under = zox_tst_terrain_voxel(world, terrain, (int3) { block.x, block.y - i, block.z });
        if (under > 0) {
            ground = i;
        }
    }
    zox_tst_check(ground, "character isn't standing on ground, no solid block within [%i] under [%ix%ix%i]",
        ground_range, block.x, block.y, block.z);
}

// tst: continue_game, continue -> load the newest save -> confirm, the player spawns in the terrain
//  - reuses load_games (its saves have no player.dat, so it places the player like a new game)
//  - then the same checks as player_spawn
byte zox_tst_continue_loaded = 0;
byte zox_tst_continue_confirmed = 0;

byte zox_tst_continue_game_ready(ecs* world) {
    if (!zox_tst_continue_loaded) {
        zox_tst_continue_loaded = zox_tst_load_games_ready(world);
        return 0;
    }
    if (!zox_tst_continue_confirmed) {
        zox_tst_waiting = "the realm menu";
        const entity player = dbg_player;
        const entity canvas = zox_get_link(world, player, CanvasLink);
        const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuRealm));
        if (!zox_valid(menu)) {
            return 0;
        }
        const entity button = zox_tst_find_click(world, menu, &button_event_menu_realm_confirm);
        zox_tst_check(button, "realm menu has no confirm button");
        if (!button) {
            return 1;
        }
        zox_tst_click(world, player, button);
        zox_tst_continue_confirmed = 1;
        zox_tst_spawn_start = zox_current_time;
        return 0;
    }
    return zox_tst_player_spawn_ready(world);
}

// tst: screenshot, saves what the player sees after spawning to tst/screenshot.ppm
//  - for looking at the rendering (i.e. lighting), bash bsh/tst.sh screenshot, then convert to png
//  - the XR simulator's two eyes side by side: bash bsh/tst.sh screenshot --xr-sim
//  - waits zox_tst_screenshot_wait after the player settles, so lights and meshes finish
//  - zox_tst_save=<realm folder> loads a copy of a real save instead (same place every run, for before / after)
extern char zox_screenshot_request[256];
const double zox_tst_screenshot_wait = 6.0;
byte zox_tst_screenshot_stage = 0;
double zox_tst_screenshot_time = 0;

byte zox_tst_screenshot_ready(ecs* world) {
    if (zox_tst_screenshot_stage == 0) {
        const char* real_save = getenv("zox_tst_save");
        const byte spawned = real_save && real_save[0] ?
            zox_tst_continue_game_ready(world) :
            zox_tst_player_spawn_ready(world);
        if (!spawned) {
            return 0;
        }
        zox_tst_screenshot_stage = 1;
        zox_tst_screenshot_time = zox_current_time;
    }
    zox_tst_waiting = "lights and meshes to finish before the screenshot";
    zox_tst_progress(zox_current_time - zox_tst_screenshot_time);
    if (zox_tst_screenshot_stage == 1) {
        if (zox_current_time - zox_tst_screenshot_time < zox_tst_screenshot_wait) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("screenshot.ppm"));
        zox_tst_screenshot_stage = 2;
        return 0;
    }
    // saved at the end of the frame it was requested in
    return !zox_screenshot_request[0];
}

void zox_tst_screenshot_check(ecs* world) {
    (void) world;
    FILE* file = fopen(zox_tst_path("screenshot.ppm"), "rb");
    zox_tst_check(file, "no screenshot saved");
    if (file) {
        fclose(file);
    }
}

#include "touch_play.c"
#include "coop_continue.c"
#include "navigator_inventory.c"
#include "gamepad_reconnect.c"
#include "gamepad_reconnect_keyboard.c"
#include "stick_deadzone.c"
#include "touch_actions.c"
#include "free_roam_pose.c"

void zox_tsts_controllers(ecs* world) {
    zox_tsts_music(world);
    zox_tsts_crouch(world);
    zox_tsts_walk_bob(world);
    zox_tsts_auto_jump(world);
    zox_tst_add("player_spawn", zox_tst_player_spawn_ready, zox_tst_player_spawn_check, 60);
    zox_tst_add("underwater", zox_tst_underwater_ready, zox_tst_underwater_check, 90);
    zox_tst_add("continue_game", zox_tst_continue_game_ready, zox_tst_player_spawn_check, 90);
    zox_tst_set_setup("continue_game", zox_tst_load_games_setup, 1);
    zox_tst_add("navigator_inventory", zox_tst_navigator_inventory_ready, zox_tst_navigator_inventory_check, 120);
    zox_tst_set_setup("navigator_inventory", zox_tst_level_setup, 0);
    zox_tst_add("stick_deadzone", zox_tst_stick_deadzone_ready, zox_tst_stick_deadzone_check, 30);
    zox_tst_set_setup("stick_deadzone", NULL, 1);
    zox_tst_add("coop_gamepad_reconnect", zox_tst_coop_gamepad_reconnect_ready, zox_tst_coop_gamepad_reconnect_check, 90);
    zox_tst_set_setup("coop_gamepad_reconnect", zox_tst_coop_level_setup, 0);
    zox_tsts_gamepad_reconnect_keyboard(world);
    zox_tst_add("coop_respawn_navigator", zox_tst_navigator_inventory_ready, zox_tst_navigator_inventory_check, 180);
    zox_tst_set_setup("coop_respawn_navigator", zox_tst_coop_respawn_navigator_setup, 0);
    zox_tst_add("coop_navigator", zox_tst_navigator_inventory_ready, zox_tst_navigator_inventory_check, 120);
    zox_tst_set_setup("coop_navigator", zox_tst_coop_navigator_setup, 0);
    zox_tst_add("coop_continue", zox_tst_coop_continue_ready, zox_tst_coop_continue_check, 120);
    zox_tst_set_setup("coop_continue", zox_tst_coop_continue_setup, 1);
    zox_tst_add("coop_join_continue", zox_tst_coop_continue_ready, zox_tst_coop_join_continue_check, 150);
    zox_tst_set_setup("coop_join_continue", zox_tst_coop_join_continue_setup, 1);
    zox_tst_add("screenshot", zox_tst_screenshot_ready, zox_tst_screenshot_check, 90);
    const char* real_save = getenv("zox_tst_save");
    if (real_save && real_save[0]) {
        zox_tst_set_setup("screenshot", zox_tst_load_games_setup, 1);
    }
    (void) world;
    zox_tst_add("action_wheel", zox_tst_action_wheel_ready, zox_tst_action_wheel_check, 60);
    zox_tst_add("item_split", zox_tst_item_split_ready, zox_tst_item_split_check, 90);
    zox_tst_add("resume_no_punch", zox_tst_resume_no_punch_ready, zox_tst_resume_no_punch_check, 90);
    zox_tst_add("touch_play", zox_tst_touch_play_ready, zox_tst_touch_play_check, 120);
    // the test level: no towns, it spawned at its home facing a wall now and then (walked [0.35])
    zox_tst_set_setup("touch_play", zox_tst_level_setup, 0);
    zox_tst_add("touch_actions", zox_tst_touch_actions_ready, zox_tst_touch_actions_check, 90);
    zox_tst_add("tooltip_flick", zox_tst_tooltip_flick_ready, zox_tst_tooltip_flick_check, 90);
    zox_tst_add("free_roam_pose", zox_tst_free_roam_pose_ready, zox_tst_free_roam_pose_check, 60);
    zox_tst_set_setup("free_roam_pose", zox_tst_void_setup, 1);    // the body alone, in the void
}

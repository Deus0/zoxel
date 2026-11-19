// tsts for local coop (--splitscreen, a second player joined at boot), the screens: tst split_screen, the huds:
//  tst split_screen_huds, the land round two streamers: tst coop_streamers
int zox_tst_coop_frames = 0;

void zox_tst_coop_level_setup() {
    zox_tst_level_setup();
    is_split_screen = 1;
}

void zox_tst_coop_setup() {
    is_split_screen = 1;
}

// a whole world with towns: the sea lowered (towns keep off water, most worlds were under it, none had towns)
void zox_tst_coop_towns_setup() {
    is_split_screen = 1;
    terrain_sea_height = 0;
}

// both players' characters, once spawned
static inline byte zox_tst_coop_playing(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the second player playing";
    return players_playing >= 2 && zox_getv(zox_players[1], PlayerState) == zox_player_state_playing &&
        zox_valid(zox_get_link(world, zox_players[1], CharacterLink));
}

// tst: coop_skybox, split screen: the skybox is drawn round each camera (RenderAtCamera)
//  - it was the first camera's child, drawn there: the second player, a way off, saw it from outside, the world past
//      its walls, cut off
//  - the test level, the players side by side: each main camera's last skybox was drawn at its own position
//  - run: bash bsh/tst.sh coop_skybox
byte zox_tst_coop_skybox_ready(ecs* world) {
    if (!zox_tst_coop_playing(world)) {
        return 0;
    }
    zox_tst_coop_frames++;
    // a frame's draws from here
    if (zox_tst_coop_frames == 60) {
        for (int i = 0; i < max_cameras; i++) {
            render_at_camera_drawn[i] = (float3) { 99999, 99999, 99999 };
        }
    }
    return zox_tst_coop_frames >= 70;
}

void zox_tst_coop_skybox_check(ecs* world) {
    const entity skybox = zox_valid(local_game) ? zox_get_link(world, local_game, Skybox) : 0;
    zox_tst_check(zox_valid(skybox), "no skybox");
    if (zox_valid(skybox)) {
        zox_tst_check(zox_has(skybox, RenderAtCamera), "the skybox isn't drawn round each camera");
    }
    zox_tst_check(main_cameras_count == 2, "[%i] main cameras, expected 2", main_cameras_count);
    if (main_cameras_count < 2 || !zox_valid(main_cameras[0]) || !zox_valid(main_cameras[1])) {
        return;
    }
    const float3 a = zox_getv(main_cameras[0], Position3D);
    const float3 b = zox_getv(main_cameras[1], Position3D);
    zox_log("[tst] cameras at [%.1fx%.1fx%.1f] and [%.1fx%.1fx%.1f], [%.1f] apart", a.x, a.y, a.z, b.x, b.y, b.z, float3_distance(a, b));
    zox_tst_check(float3_distance(a, b) > 1, "the players' cameras are [%.2f] apart, the test can't tell their skyboxes apart",
        float3_distance(a, b));
    for (int i = 0; i < 2; i++) {
        const float3 camera = zox_getv(main_cameras[i], Position3D);
        const float3 drawn = render_at_camera_drawn[i];
        // the camera moves a little between its draw and now (head bob)
        zox_tst_check(float3_distance(camera, drawn) < 0.5f, "camera [%i]'s skybox drawn at [%.1fx%.1fx%.1f], [%.1f] from it",
            i + 1, drawn.x, drawn.y, drawn.z, float3_distance(camera, drawn));
    }
}

// tst: coop_homes, a new game with two players: they spawn in the same town, each in front of their own home's door
//  - each picked a random region and town, so they started far apart; a home each (player_town_finder_system)
//  - a whole world (towns aren't in the test level), seed zox_tst_seed
//  - run: bash bsh/tst.sh coop_homes
entity zox_tst_coop_homes[2];
float zox_tst_coop_home_distances[2];
uint zox_tst_coop_town_homes = 0;

// the home whose door front column is closest to a position (in blocks, x and z), of the terrain's towns
static inline entity zox_tst_coop_closest_home(ecs* world, entity terrain, float3 position, float* distance) {
    const float scale = zox_getv(terrain, BlockScale);
    const float2 at = (float2) { position.x / scale, position.z / scale };
    entity closest = 0;
    *distance = 99999;
    entity regions[zox_children_capacity];
    const uint regions_count = zox_get_children_by_id(world, terrain, regions, zox_children_capacity, zox_id(Region));
    for (uint r = 0; r < regions_count; r++) {
        entity towns[zox_children_capacity];
        const uint towns_count = zox_get_children_by_id(world, regions[r], towns, zox_children_capacity, zox_id(Town));
        for (uint t = 0; t < towns_count; t++) {
            entity homes[zox_children_capacity];
            const uint homes_count = zox_get_children_by_id(world, towns[t], homes, zox_children_capacity, zox_id(Home));
            for (uint h = 0; h < homes_count; h++) {
                const int2 door = home_door_front(world, homes[h]);
                // the column's middle
                const float dx = door.x + 0.5f - at.x, dz = door.y + 0.5f - at.y;
                const float d = sqrtf(dx * dx + dz * dz);
                if (d < *distance) {
                    *distance = d;
                    closest = homes[h];
                }
            }
        }
    }
    return closest;
}

byte zox_tst_coop_homes_ready(ecs* world) {
    if (!zox_tst_coop_playing(world)) {
        return 0;
    }
    // where they spawned, before they move
    const entity terrain = local_terrain;
    for (int i = 0; i < 2; i++) {
        const entity character = zox_get_link(world, zox_players[i], CharacterLink);
        zox_tst_coop_homes[i] = zox_valid(terrain) && zox_valid(character) ?
            zox_tst_coop_closest_home(world, terrain, zox_getv(character, Position3D), &zox_tst_coop_home_distances[i]) : 0;
    }
    if (zox_valid(zox_tst_coop_homes[0])) {
        entity homes[zox_children_capacity];
        zox_tst_coop_town_homes = zox_get_children_by_id(world, zox_get_parent(world, zox_tst_coop_homes[0]), homes,
            zox_children_capacity, zox_id(Home));
    }
    return 1;
}

void zox_tst_coop_homes_check(ecs* world) {
    for (int i = 0; i < 2; i++) {
        zox_tst_check(zox_valid(zox_tst_coop_homes[i]), "player [%i] has no home near (no towns?)", i + 1);
        if (zox_valid(zox_tst_coop_homes[i])) {
            zox_log("[tst] player [%i] spawned [%.1f] blocks from [%s]'s door, of [%s]", i + 1, zox_tst_coop_home_distances[i],
                zox_get_name(zox_tst_coop_homes[i]), zox_get_name(zox_get_parent(world, zox_tst_coop_homes[i])));
            zox_tst_check(zox_tst_coop_home_distances[i] < 1.5f, "player [%i] spawned [%.1f] blocks from the closest home's door",
                i + 1, zox_tst_coop_home_distances[i]);
        }
    }
    if (!zox_valid(zox_tst_coop_homes[0]) || !zox_valid(zox_tst_coop_homes[1])) {
        return;
    }
    zox_tst_check(zox_get_parent(world, zox_tst_coop_homes[0]) == zox_get_parent(world, zox_tst_coop_homes[1]),
        "the players spawned in different towns");
    zox_log("[tst] their town has [%i] homes", zox_tst_coop_town_homes);
    if (zox_tst_coop_town_homes >= 2) {
        zox_tst_check(zox_tst_coop_homes[0] != zox_tst_coop_homes[1], "both players spawned at the same home, of [%i]",
            zox_tst_coop_town_homes);
    }
}

// tst: coop_saves, each player saves to its own file: player.dat, player2.dat
//  - they all saved and loaded player.dat, a continued coop game put every player at the last one saved
//  - waits until both files hold their character's position (it saves when it moves, character_save_system)
//  - run: bash bsh/tst.sh coop_saves
float zox_tst_coop_save_distances[2];

byte zox_tst_coop_saves_ready(ecs* world) {
    if (!zox_tst_coop_playing(world) || !zox_valid(local_realm)) {
        return 0;
    }
    zox_tst_waiting = "both players' saves to hold their positions";
    const FolderPath* path = zox_get(local_realm, FolderPath);
    byte saved = 0;
    for (int i = 0; i < 2; i++) {
        char name[32];
        SaveDataCharacter save;
        const entity character = zox_get_link(world, zox_players[i], CharacterLink);
        zox_tst_coop_save_distances[i] = 99999;
        if (zox_valid(character) && load_file_struct(path->value, get_player_save_file(zox_players[i], name, sizeof(name)),
            &save, sizeof(SaveDataCharacter))) {
            zox_tst_coop_save_distances[i] = float3_distance(save.position, zox_getv(character, Position3D));
            saved += zox_tst_coop_save_distances[i] < 0.1f;
        }
    }
    return saved == 2;
}

void zox_tst_coop_saves_check(ecs* world) {
    char name[32];
    zox_tst_check(!strcmp(get_player_save_file(zox_players[0], name, sizeof(name)), "player.dat"),
        "the first player saves to [%s]", name);
    zox_tst_check(!strcmp(get_player_save_file(zox_players[1], name, sizeof(name)), "player2.dat"),
        "the second player saves to [%s]", name);
    for (int i = 0; i < 2; i++) {
        zox_tst_check(zox_tst_coop_save_distances[i] < 0.1f, "player [%i]'s save is [%.2f] from its character",
            i + 1, zox_tst_coop_save_distances[i]);
    }
    const entity a = zox_get_link(world, zox_players[0], CharacterLink);
    const entity b = zox_get_link(world, zox_players[1], CharacterLink);
    if (zox_valid(a) && zox_valid(b)) {
        const float apart = float3_distance(zox_getv(a, Position3D), zox_getv(b, Position3D));
        zox_log("[tst] the players are [%.1f] apart", apart);
        zox_tst_check(apart > 0.5f, "the players are [%.2f] apart, the test can't tell their saves apart", apart);
    }
}

// tst: coop_billboards, split screen: a 3D ui faces each camera that draws it, not only the closest one
//  - billboard_system turns it to the closest camera; each camera's draw turns it on to face itself (element3_draw)
//  - the test level, a canvas between the players, the second camera turned a quarter round
//  - for each camera, the matrix it draws the canvas's frame with: facing as the camera does, at the same place, level
//      (the camera's roll left out, a VR head's tilt tilted them)
//  - run: bash bsh/tst.sh coop_billboards
entity zox_tst_coop_billboard = 0;
entity zox_tst_coop_billboard_frame = 0;

byte zox_tst_coop_billboards_ready(ecs* world) {
    if (!zox_tst_coop_playing(world) || main_cameras_count < 2) {
        return 0;
    }
    if (!zox_tst_coop_billboard) {
        const float3 a = zox_getv(main_cameras[0], Position3D);
        const float3 b = zox_getv(main_cameras[1], Position3D);
        const float3 at = float3_add(float3_scale(float3_add(a, b), 0.5f), (float3) { 0, 1, 0 });
        zox_tst_coop_billboard = spawn_canvas3(world, prefab_canvas3, at, 2, int2_single(64));
        zox_tst_coop_billboard_frame = spawn_element3(world, prefab_frame3, zox_tst_coop_billboard, float2_centre,
            int2_zero, int2_single(64));
        zox_tst_coop_frames = 0;
    }
    zox_tst_waiting = "the billboard to turn";
    return ++zox_tst_coop_frames >= 20;
}

void zox_tst_coop_billboards_check(ecs* world) {
    const entity canvas = zox_tst_coop_billboard, frame = zox_tst_coop_billboard_frame;
    zox_tst_check(zox_valid(canvas) && zox_valid(frame), "no billboard");
    if (!zox_valid(canvas) || !zox_valid(frame)) {
        return;
    }
    // the second camera looks another way (the players turned apart), until the next frame turns it back
    //  and tilts its head (a VR head's roll), the billboard stays level
    zox_setv(main_cameras[1], Rotation3D, quaternion_rotate(quaternion_from_axis_angle((float3) { 0, 1, 0 },
        degrees_to_radians * 90), quaternion_from_axis_angle((float3) { 0, 0, 1 }, degrees_to_radians * 30)));
    const entity was = renderer_camera;
    const float4x4 built = zox_getv(frame, TransformMatrix);
    for (int i = 0; i < 2; i++) {
        renderer_camera = main_cameras[i];
        float4x4 turn;
        const float4x4 model = element3_billboard_turn(world, canvas, &turn) ? float4x4_multiply(built, turn) : built;
        const float4x4 facing = float4x4_rotation(quaternion_without_roll(zox_getv(main_cameras[i], Rotation3D)));
        const float3 forward = float3_normalize((float3) { model.z.x, model.z.y, model.z.z });
        const float3 expected = float3_normalize((float3) { facing.z.x, facing.z.y, facing.z.z });
        const float moved = float3_distance((float3) { model.w.x, model.w.y, model.w.z }, (float3) { built.w.x, built.w.y, built.w.z });
        zox_log("[tst] camera [%i] draws it facing [%.2fx%.2fx%.2f], it faces [%.2fx%.2fx%.2f], moved [%.3f]", i + 1,
            forward.x, forward.y, forward.z, expected.x, expected.y, expected.z, moved);
        zox_tst_check(float3_distance(forward, expected) < 0.01f, "camera [%i] draws the billboard facing another way", i + 1);
        zox_tst_check(moved < 0.01f, "camera [%i] draws the billboard [%.3f] from where it is", i + 1, moved);
        const float3 right = float3_normalize((float3) { model.x.x, model.x.y, model.x.z });
        zox_tst_check(fabsf(right.y) < 0.01f, "camera [%i] draws the billboard tilted, its right [%.2f] off level",
            i + 1, right.y);
    }
    renderer_camera = was;
}

void zox_tsts_coop(ecs* world) {
    (void) world;
    zox_tst_add("coop_skybox", zox_tst_coop_skybox_ready, zox_tst_coop_skybox_check, 120);
    zox_tst_set_setup("coop_skybox", zox_tst_coop_level_setup, 0);
    zox_tst_add("coop_homes", zox_tst_coop_homes_ready, zox_tst_coop_homes_check, 150);
    zox_tst_set_setup("coop_homes", zox_tst_coop_towns_setup, 0);
    zox_tst_add("coop_billboards", zox_tst_coop_billboards_ready, zox_tst_coop_billboards_check, 120);
    zox_tst_set_setup("coop_billboards", zox_tst_coop_level_setup, 0);
    zox_tst_add("coop_saves", zox_tst_coop_saves_ready, zox_tst_coop_saves_check, 120);
    zox_tst_set_setup("coop_saves", zox_tst_coop_level_setup, 0);
}

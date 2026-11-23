extern char zox_screenshot_request[256];
// tst: split_screen, local coop: a second player joins on the main menu, the screen splits only in game
//  - a test gamepad (zox_tst_spawn_pad) of the first player's, who isn't using it: its A on the main menu joins a
//      second player with it (the first player stays on theirs), the join ui at the bottom shows P1 and P2, the
//      second player has no cameras or canvas, one camera fills the window
//  - new game > enter realm: as it loads the second player gets its cameras and canvas, each player's in their half
//      of the window, and a character each (their huds don't overlap: tst split_screen_huds), alike: its bones, its
//      eye at its head, its body drawn the same (zox_tst_coop_players_alike)
//  - pause > end game > the main menu: the second player's screen goes, the first player's fills the window again
//  - its B leaves: the gamepad is the first player's again, the join ui shows P1 alone
//  - tst/split_screen/split_screen.ppm: both players' screens in game, to look at
//  - run: bash bsh/tst.sh split_screen
// each player's character: a skeleton with its bones and a head, its camera at its head, and the second as the first:
//  each bone the same (size, joint, local place), posed the same from its camera, and its body's mesh the same (its
//  vertices on the same bones: the second's clothes rebuilt its bones but not its voxels, drawn deformed)
static inline int zox_tst_coop_bones(ecs* world, entity character, entity* bones) {
    const entity skeleton = zox_valid(character) ? get_skeleton(world, character) : 0;
    return zox_valid(skeleton) ? (int) zox_get_children_by_id_recursive(world, skeleton, bones, 256, zox_id(Bone), 0) : 0;
}

// its skeleton's drawn mesh (not stale)
static inline entity zox_tst_coop_mesh(ecs* world, entity character) {
    const entity skeleton = zox_valid(character) ? get_skeleton(world, character) : 0;
    entity found = 0;
    if (zox_valid(skeleton)) {
        iter it2 = zox_children(world, skeleton);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                const entity m = it2.entities[j];
                if (zox_has(m, SkeletonMesh) && !zox_has(m, SkeletonMeshStale) && !zox_getv(m, RenderDisabled)) {
                    found = m;
                }
            }
        }
    }
    return found;
}

static inline void zox_tst_coop_players_alike(ecs* world, const char* when) {
    if (players_playing < 2) {
        return;
    }
    entity characters[2], cameras[2], meshes[2];
    entity bones[2][256];
    int counts[2];
    float heights[2];
    for (int i = 0; i < 2; i++) {
        characters[i] = zox_get_link(world, zox_players[i], CharacterLink);
        cameras[i] = zox_get_link(world, zox_players[i], CameraLink);
        zox_tst_check(zox_valid(characters[i]) && zox_valid(cameras[i]), "%s: player [%i] has no character or camera", when, i + 1);
        if (!zox_valid(characters[i]) || !zox_valid(cameras[i])) {
            return;
        }
        counts[i] = zox_tst_coop_bones(world, characters[i], bones[i]);
        meshes[i] = zox_tst_coop_mesh(world, characters[i]);
        const entity skeleton = get_skeleton(world, characters[i]);
        const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
        const float3 c = zox_getv(characters[i], Position3D), b = zox_getv(characters[i], Bounds3D);
        const float3 eye = zox_getv(cameras[i], Position3D);
        const float flat = sqrtf((eye.x - c.x) * (eye.x - c.x) + (eye.z - c.z) * (eye.z - c.z));
        heights[i] = eye.y - (c.y - b.y);
        zox_log("[tst] %s player [%i] bones [%i] eye over its feet [%.2f] of [%.2f] mesh vertices [%i]", when, i + 1,
            counts[i], heights[i], b.y * 2, zox_valid(meshes[i]) ? zox_get(meshes[i], MeshVertices)->length : -1);
        zox_tst_check(counts[i] > 4 && zox_valid(head), "%s: player [%i] has [%i] bones, head [%i]", when, i + 1, counts[i], zox_valid(head));
        zox_tst_check(heights[i] > b.y && heights[i] < b.y * 2 + 0.1f && flat < b.x * 2,
            "%s: player [%i]'s eye is [%.2f] over its feet, [%.2f] across, not at its head", when, i + 1, heights[i], flat);
    }
    zox_tst_check(counts[0] == counts[1], "%s: the players have [%i] and [%i] bones", when, counts[0], counts[1]);
    zox_tst_check(fabsf(heights[0] - heights[1]) < 0.05f, "%s: the players' eyes are at [%.2f] and [%.2f] over their feet",
        when, heights[0], heights[1]);
    int off = 0, posed = 0;
    for (int j = 0; j < counts[0] && j < counts[1]; j++) {
        const entity a = bones[0][j], b = bones[1][j];
        off += float3_distance(zox_getv(a, BoneSize), zox_getv(b, BoneSize)) > 0.001f ||
            float3_distance(zox_getv(a, BonePosition), zox_getv(b, BonePosition)) > 0.001f ||
            float3_distance(zox_getv(a, LocalPosition3D), zox_getv(b, LocalPosition3D)) > 0.02f;
        float3 seen[2];
        for (int i = 0; i < 2; i++) {
            const float3 d = float3_subtract(zox_getv(bones[i][j], Position3D), zox_getv(cameras[i], Position3D));
            seen[i] = float4_rotate_float3(quaternion_inverse(zox_getv(cameras[i], Rotation3D)), d);
        }
        posed += float3_distance(seen[0], seen[1]) > 0.03f;
    }
    zox_tst_check(!off, "%s: [%i] of the second player's bones differ from the first's", when, off);
    zox_tst_check(!posed, "%s: [%i] of the second player's bones are posed off the first's", when, posed);
    zox_tst_check(zox_valid(meshes[0]) && zox_valid(meshes[1]), "%s: a player's body has no mesh", when);
    if (!zox_valid(meshes[0]) || !zox_valid(meshes[1])) {
        return;
    }
    const MeshVertices* va = zox_get(meshes[0], MeshVertices);
    const MeshVertices* vb = zox_get(meshes[1], MeshVertices);
    const BoneIndexes* wa = zox_get(meshes[0], BoneIndexes);
    const BoneIndexes* wb = zox_get(meshes[1], BoneIndexes);
    int rebound = 0;
    for (int k = 0; k < va->length && k < vb->length && k < wa->length && k < wb->length; k++) {
        rebound += wa->value[k].x != wb->value[k].x || wa->value[k].y != wb->value[k].y;
    }
    zox_tst_check(va->length == vb->length, "%s: the players' bodies have [%i] and [%i] vertices", when, va->length, vb->length);
    zox_tst_check(!rebound, "%s: [%i] of the second player's vertices are on other bones than the first's", when, rebound);
}

// tst: split_screen_game, the same in the whole game (not the test level), at Block Res 5 (a play tester's), its
//  gamepad connecting on the main menu (the app's, as one plugged in), not the first player's
byte zox_tst_sscreen_connect = 0;
void zox_tst_split_screen_game_setup() {
    block_depth = 5;
    zox_tst_sscreen_connect = 1;
}

int zox_tst_sscreen_stage = 0;
int zox_tst_sscreen_frames = 0;
int zox_tst_sscreen_menu_frames = 0;

static inline void zox_tst_sscreen_next() {
    zox_tst_sscreen_stage++;
    zox_tst_sscreen_frames = 0;
}

// the join ui's player labels (P1, P2..)
static inline int zox_tst_sscreen_join_labels(ecs* world, entity canvas) {
    int count = 0;
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity e = it2.entities[j];
            if (!zox_has(e, PlayerJoinUI) || !zox_has(e, TextData)) {
                continue;
            }
            char* text = get_zext_text(zox_get(e, TextData));
            count += text && text[0] == 'P';
            free(text);
        }
    }
    return count;
}

// each player's cameras and canvas in their cell of the window
static inline void zox_tst_sscreen_check_screens(ecs* world, int count, const char* when) {
    const int2 window = zox_getv(main_app, WindowSize);
    zox_tst_check(main_cameras_count == count, "%s: [%i] main cameras, expected [%i]", when, main_cameras_count, count);
    for (int i = 0; i < count && i < players_playing; i++) {
        const float4 cell = viewport_grid_cell(i, count, 1);
        const entity player = zox_players[i];
        const entity camera = zox_get_link(world, player, CameraLink);
        const entity canvas = zox_get_link(world, player, CanvasLink);
        zox_tst_check(zox_valid(camera) && camera == main_cameras[i], "%s: player [%i]'s camera isn't main camera [%i]", when, i + 1, i);
        zox_tst_check(zox_valid(canvas), "%s: player [%i] has no canvas", when, i + 1);
        if (!zox_valid(camera) || !zox_valid(canvas)) {
            continue;
        }
        zox_tst_camera_in_cell(world, camera, window, cell, when);
        zox_tst_camera_in_cell(world, zox_get_link(world, canvas, CameraLink), window, cell, when);
        const int2 size = zox_getv(canvas, LayoutSize);
        const int2 expected = screen_to_canvas_size(window, cell);
        zox_tst_check(int2_equals(size, expected), "%s: player [%i]'s canvas [%ix%i], expected [%ix%i]",
            when, i + 1, size.x, size.y, expected.x, expected.y);
    }
}

byte zox_tst_split_screen_ready(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_sscreen_frames++;
    if (zox_tst_sscreen_frames < zox_tst_menus_settle_frames) {
        return 0;
    }
    zox_tst_progress(zox_tst_sscreen_stage * 1000 + zox_tst_sscreen_frames);
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity new_realm = zox_tst_menu(world, canvas, zox_id(MenuNewRealm));
    const entity paused = get_pause_menu(world);
    const byte state = zox_tst_game_state(world, player);
    switch (zox_tst_sscreen_stage) {
        case 0:
            zox_tst_waiting = "the main menu";
            if (!zox_tst_main_menu(world, player, canvas)) {
                return 0;
            }
            memset(zox_tst_pad_held, 0, sizeof(zox_tst_pad_held));
            zox_tst_pad = zox_tst_spawn_pad(world, player);
            if (zox_tst_sscreen_connect) {
                zox_set_parent(world, zox_tst_pad, main_app);   // plugged in: the app's till a player joins with it
            }
            break;
        case 1:
            zox_tst_waiting = "the join ui to show both players";
            // A, a few frames on
            zox_tst_pad_held[zox_btn_a] = zox_tst_sscreen_frames >= zox_tst_menus_settle_frames + 5 &&
                zox_tst_sscreen_frames < zox_tst_menus_settle_frames + 9;
            if (zox_tst_sscreen_join_labels(world, canvas) != 2) {
                if (zox_tst_sscreen_frames <= 120) {
                    return 0;
                }
                zox_tst_check(0, "the join ui shows [%i] players, not 2", zox_tst_sscreen_join_labels(world, canvas));
                return 1;
            }
            zox_tst_check(players_playing == 2, "[%i] players after joining", players_playing);
            if (players_playing == 2) {
                zox_tst_check(zox_get_link(world, zox_players[1], GamepadLink) == zox_tst_pad, "the second player didn't join with the gamepad");
                zox_tst_check(zox_get_parent(world, zox_tst_pad) == zox_players[1], "the gamepad isn't the second player's");
            }
            zox_tst_check(zox_getv(player, DeviceMode) != zox_device_mode_gamepad, "the first player switched to the gamepad that joined");
            zox_tst_check(!zox_get_link(world, zox_players[1], CanvasLink) && !zox_get_link(world, zox_players[1], CameraLink),
                "the second player has a screen on the main menu");
            zox_tst_sscreen_check_screens(world, 1, "main menu");
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_new_game, "new game")) {
                return 1;
            }
            break;
        case 2:
            zox_tst_waiting = "the new realm menu";
            if (!new_realm) {
                return 0;
            }
            zox_tst_check(!zox_tst_sscreen_join_labels(world, canvas), "the join ui stayed after the main menu");
            if (!zox_tst_menus_click(world, player, new_realm, &on_confirmed_new_realm, "enter realm")) {
                return 1;
            }
            break;
        case 3:
            zox_tst_waiting = "both players playing";
            if (state != zox_game_state_playing || zox_getv(player, PlayerState) != zox_player_state_playing ||
                zox_getv(zox_players[1], PlayerState) != zox_player_state_playing) {
                zox_tst_progress(zox_tst_sscreen_stage * 1000 + (zox_gpu_buffers_live + zox_gpu_buffers_freed) * 1000.0);
                return 0;
            }
            if (zox_tst_sscreen_frames < 900) {
                return 0;
            }
            // its shot taken before it pauses, both players' eyes open (it was the pause's first frame, eyes shut)
            if (zox_tst_sscreen_frames == 900) {
                snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("split_screen.ppm"));
                return 0;
            }
            if (zox_screenshot_request[0]) {
                return 0;
            }
            zox_tst_sscreen_check_screens(world, 2, "in game");
            zox_tst_check(zox_valid(zox_get_link(world, zox_players[1], CharacterLink)), "the second player has no character");
            zox_tst_coop_players_alike(world, "split_screen");

            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        case 4:
            zox_tst_waiting = "the pause menu";
            if (!paused) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, paused, &button_event_end_game, "end game")) {
                return 1;
            }
            break;
        case 5:
            zox_tst_waiting = "the main menu after ending the game";
            // its screens resize the frame after the menu spawns (viewport_resize_system): wait from the menu
            if (!main_menu || state != zox_game_none) {
                zox_tst_sscreen_menu_frames = 0;
                return 0;
            }
            if (++zox_tst_sscreen_menu_frames < 60) {
                return 0;
            }
            zox_tst_check(!zox_get_link(world, zox_players[1], CanvasLink) && !zox_get_link(world, zox_players[1], CameraLink),
                "the second player's screen stayed after the game");
            zox_tst_sscreen_check_screens(world, 1, "back at the main menu");
            zox_tst_check(zox_tst_sscreen_join_labels(world, canvas) == 2, "the join ui doesn't show both players again");
            break;
        case 6:
            zox_tst_waiting = "the join ui to show the first player alone";
            // B, a few frames on
            zox_tst_pad_held[zox_btn_b] = zox_tst_sscreen_frames >= zox_tst_menus_settle_frames + 5 &&
                zox_tst_sscreen_frames < zox_tst_menus_settle_frames + 9;
            if (zox_tst_sscreen_join_labels(world, canvas) != 1) {
                if (zox_tst_sscreen_frames <= 120) {
                    return 0;
                }
                zox_tst_check(0, "the join ui shows [%i] players after leaving, not 1", zox_tst_sscreen_join_labels(world, canvas));
                return 1;
            }
            zox_tst_check(players_playing == 1, "[%i] players after leaving", players_playing);
            zox_tst_check(zox_get_parent(world, zox_tst_pad) == player, "the gamepad isn't the first player's again");
            zox_tst_pad_held[zox_btn_b] = 0;
            return 1;
    }
    zox_tst_sscreen_next();
    return 0;
}

void zox_tst_split_screen_check(ecs* world) {
    (void) world;
}

// A test's player character in the void: no game started, no terrain, its body and first person view on their own
//  (zox_tst_void_setup, then zox_tst_player_spawn_ready as any test): to test the body's poses and what's held,
//  isolated from the terrain's streaming and physics
//  - on the main menu (a menus test, no --autostart): a realm of its own (seed zox_tst_seed, else 666), the game
//    linking it, its items generated
//  - the player's character (spawn_character3_player, no terrain) at a spot far past the menu's scene, kept there by
//    its own tags: DisableGravity (physics3's gravity skips it), nothing under it to land on; no InsideBlock or
//    raycast (no terrain to be in or aim at, as tst body_views)
//  - its camera on its head, first person (attach_camera_to_character), the player playing (set straight: not its
//    state machine's way in, which waits on terrain and spawns the game's ui), the menu's ui hidden (nohud)
extern byte nohud;
const float3 zox_tst_void_spot = { 0, 1000, 0 };
byte zox_tst_void_stage = 0;
int zox_tst_void_frames = 0;
entity zox_tst_void_realm = 0;

static inline byte zox_tst_void_realm_items(ecs* world, entity realm) {
    int items = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            items += zox_has(it.entities[i], Item);
        }
    }
    return items > 0;
}

byte zox_tst_void_player_ready(ecs* world) {
    zox_tst_void_frames++;
    zox_tst_progress(zox_tst_void_stage * 1000 + int_min(zox_tst_void_frames, 999));
    const entity player = dbg_player;
    if (zox_tst_void_stage == 0) {
        zox_tst_waiting = "the game, app and player to spawn a realm into";
        if (!zox_valid(local_game) || !zox_valid(main_app) || !zox_valid(player) || zox_tst_void_frames < 5) {
            return 0;
        }
        const char* seed_text = getenv("zox_tst_seed");
        const lint seed = seed_text && seed_text[0] ? atoll(seed_text) : 666;
        zox_tst_void_realm = spawn_realm(world, local_game, seed);
        zox_setv(zox_tst_void_realm, GenerateRealm, zox_generate_realm_start);
        zox_set_link(world, local_game, RealmLink, zox_tst_void_realm);
        zox_tst_void_stage = 1;
        return 0;
    }
    const entity realm = zox_tst_void_realm;
    if (zox_tst_void_stage == 1) {
        zox_tst_waiting = "the void's realm to generate its items";
        if (zox_getv(realm, GenerateRealm) || !zox_tst_void_realm_items(world, realm)) {
            return 0;
        }
        const entity character = spawn_character3_player(world, prefab_character3_player, player, realm, 0,
            zox_getv(realm, Seed), zox_tst_void_spot, quaternion_identity, "void_player");
        zox_add(character, DisableGravity);
        zox_remove(character, InsideBlock);
        zox_remove(character, RaycastRange);
        zox_remove(character, RaycastVoxelData);
        const entity camera = zox_get_link(world, player, CameraLink);
        if (zox_valid(camera)) {
            zox_remove(camera, Roaming);
            attach_camera_to_character(world, camera, character);
            zox_setv(camera, CameraState, zox_camera_state_first_person);
        }
        zox_setv(player, PlayerState, zox_player_state_playing);
        nohud = 1;
        zox_tst_void_stage = 2;
        zox_tst_void_frames = 0;
        return 0;
    }
    const entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character)) {
        zox_tst_waiting = "the void's player character";
        return 0;
    }
    // once ready it stays so: the test may then take its camera off (free roam) or change its body
    if (zox_tst_void_stage == 3) {
        return 1;
    }
    // its body built (its skeleton's mesh drawn) and its camera on its head
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity camera = zox_get_link(world, character, CameraLink);
    static char waiting[256];
    snprintf(waiting, sizeof(waiting), "the void's player character's body and first person camera (skeleton [%i] head [%i] camera [%i] its state [%i] on the head [%i])",
        zox_valid(skeleton), zox_valid(head), zox_valid(camera), zox_valid(camera) ? zox_getv(camera, CameraState) : -1,
        zox_valid(camera) && zox_get_parent(world, camera) == head);
    zox_tst_waiting = waiting;
    if (zox_valid(head) && zox_valid(camera) && zox_get_parent(world, camera) == head && zox_tst_void_frames > 30) {
        zox_tst_void_stage = 3;
    }
    return zox_tst_void_stage == 3;
}

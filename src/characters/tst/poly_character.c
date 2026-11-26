// tst: poly_character, a character3 on a poly terrain: no voxel parts (zeland doesn't add them), a capsule collider
//  (spawn_capsule_collider), dropped over the island's middle it lands and rests on the surface
//  - a realm of its own from the menus (as tst void_player), the terrain its child (its clock the realm's)
//  - run: zox_tst_game=zeland bash bsh/tst.sh poly_character
#define zox_tst_pc_radius 0.3f
#define zox_tst_pc_height 1.0f
byte zox_tst_pc_stage = 0;
int zox_tst_pc_frames = 0;
entity zox_tst_pc_realm = 0;
entity zox_tst_pc_terrain = 0;
entity zox_tst_pc_character = 0;

// the character's feet over the terrain's surface under it (0 resting on it)
static inline float zox_tst_pc_gap(ecs* world) {
    const float3 position = float3_sub(zox_getv(zox_tst_pc_character, Position3D), zox_getv(zox_tst_pc_terrain, Position3D));
    const PolyHeights* heights = zox_get(zox_tst_pc_terrain, PolyHeights);
    const float surface = poly_height_at(heights->value, zox_getv(zox_tst_pc_terrain, PolyCells),
        zox_getv(zox_tst_pc_terrain, Size2), position.x, position.z);
    return position.y - zox_tst_pc_height / 2 - surface;
}

byte zox_tst_poly_character_ready(ecs* world) {
    zox_tst_pc_frames++;
    zox_tst_progress(zox_tst_pc_stage * 1000 + int_min(zox_tst_pc_frames, 999));
    if (zox_tst_pc_stage == 0) {
        zox_tst_waiting = "the game to spawn a realm into";
        if (!zox_valid(local_game) || zox_tst_pc_frames < 5) {
            return 0;
        }
        zox_tst_pc_realm = spawn_realm(world, local_game, 666);
        zox_setv(zox_tst_pc_realm, GenerateRealm, zox_generate_realm_start);
        zox_tst_pc_stage = 1;
        return 0;
    }
    if (zox_tst_pc_stage == 1) {
        zox_tst_waiting = "the realm to generate";
        if (zox_getv(zox_tst_pc_realm, GenerateRealm)) {
            return 0;
        }
        zox_tst_pc_terrain = spawn_poly_terrain(world, zox_tst_pc_realm, 666);
        zox_tst_pc_stage = 2;
        return 0;
    }
    if (zox_tst_pc_stage == 2) {
        zox_tst_waiting = "the terrain's heights";
        const PolyHeights* heights = zox_get(zox_tst_pc_terrain, PolyHeights);
        if (!heights || !heights->length) {
            return 0;
        }
        const float2 size = zox_getv(zox_tst_pc_terrain, Size2);
        const float surface = poly_height_at(heights->value, zox_getv(zox_tst_pc_terrain, PolyCells), size,
            size.x / 2, size.y / 2);
        zox_tst_pc_character = spawn_character3(world, prefab_character3, zox_tst_pc_realm, zox_tst_pc_terrain, 666,
            0, 0, (float3) { 0, surface + 3, 0 }, quaternion_identity, "poly_character");
        spawn_capsule_collider(world, zox_tst_pc_character, zox_tst_pc_radius, zox_tst_pc_height, float3_zero);
        zox_tst_pc_stage = 3;
        zox_tst_pc_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the character to land on the island";
    const float gap = zox_tst_pc_gap(world);
    zox_tst_check(gap > -0.05f, "the character fell [%f] under the surface", gap);
    return zox_getv(zox_tst_pc_character, Grounded) && fabsf(gap) < 0.03f &&
        fabsf(zox_getv(zox_tst_pc_character, Velocity3D).y) < 0.05f && zox_tst_pc_frames > 30;
}

void zox_tst_poly_character_check(ecs* world) {
    const entity e = zox_tst_pc_character;
    zox_tst_check(zox_getv(e, Grounded), "the character isn't grounded");
    zox_tst_check(fabsf(zox_tst_pc_gap(world)) < 0.03f, "the character rests [%f] off the surface", zox_tst_pc_gap(world));
    zox_tst_check(!zox_has(e, LinkChunk) && !zox_has(e, InsideBlock) && !zox_has(e, RaycastVoxelData) &&
        !zox_has(e, TerrainGravity), "the character has voxel terrain parts");
    zox_tst_check(zox_get_link(world, e, ClockLink) == zox_tst_pc_terrain &&
        zox_get_link(world, zox_tst_pc_terrain, ClockLink) == zox_tst_pc_realm, "its clock isn't the terrain's, the realm's");
    const uint errors = zox_log_errors_count - zox_tst_errors_start;
    zox_tst_check(errors == 0, "[%u] errors logged", errors);
}

void zox_tsts_poly_character(ecs* world) {
    (void) world;
    zox_tst_add("poly_character", zox_tst_poly_character_ready, zox_tst_poly_character_check, 60);
    zox_tst_set_setup("poly_character", NULL, 1);
}

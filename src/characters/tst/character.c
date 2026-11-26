// A fixture: a character with a skeleton, no game, no player
//  - a realm of its own (world_only.c, its body parts generated), the character spawned in the void far from the
//      menu's scene, or on a terrain when given one (i.e. the air chunk's, src/terrains/voxel/tst/air_chunk.c)
//  - register the test with zox_tst_set_setup(name, NULL, 1): it runs on the menus
//  - held still by the test if it needs (no ground in the void)
entity zox_tst_body = 0;
int zox_tst_body_frames = 0;

// the character once its skeleton is there, 0 until then
entity zox_tst_body_ready(ecs* world, entity terrain, float3 position) {
    zox_tst_body_frames++;
    if (!zox_tst_world_ready(world)) {
        return 0;
    }
    const entity realm = zox_tst_world_realm;
    if (!zox_tst_body) {
        zox_tst_waiting = "the realm's body parts";
        if (!zox_tst_shapes_find(world, realm, "Chest") || !zox_tst_shapes_find(world, realm, "Head")) {
            return 0;
        }
        // in the void: the world's own terrain, nothing streams there, so no chunks to be in (systems want one linked)
        zox_tst_body = spawn_character3(world, prefab_character3_skeleton, realm, terrain ? terrain : zox_tst_world_terrain,
            666, 0, 0, position, quaternion_identity, "tst_body");
        // built and dressed as the player is (character_player_equips_system), without the player's controls
        zox_add(zox_tst_body, PlayerCharacter);
        zox_tst_body_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the character's skeleton";
    return zox_valid(zox_get_link(world, zox_tst_body, SkeletonLink)) ? zox_tst_body : 0;
}

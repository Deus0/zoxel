// tst: chest_weights, a humanoid's chest moves with its chest (bones/sys/generate.c): its vertices inside only the
//  chest's own box are weighted to the chest, not a shoulder whose (enlarged) box reaches over its sides (the ribs
//  came away from the body when the arms swung)
//  - spawns a humanoid by the player, waits for its body, checks its active skeleton mesh's weights
//  - run: bash bsh/tst.sh chest_weights
int zox_tst_cweights_frames = 0;
entity zox_tst_cweights_npc = 0;

byte zox_tst_chest_weights_ready(ecs* world) {
    const entity player = dbg_player;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(character) || zox_getv(player, PlayerState) != zox_player_state_playing) {
        zox_tst_waiting = "playing";
        return 0;
    }
    zox_tst_cweights_frames++;
    zox_tst_progress(zox_tst_cweights_frames);
    zox_tst_waiting = "a humanoid's body";
    if (zox_tst_cweights_frames == 10) {
        const entity realm = zox_get_link(world, character, RealmLink);
        float3 position = zox_getv(character, Position3D);
        position.x += 2;
        zox_tst_cweights_npc = spawn_character3_npc(world, zox_tst_realm_humanoid(world, realm), realm,
            zox_get_link(world, character, TerrainLink), 1, 0, 0, position, quaternion_identity, NULL);
    }
    return zox_tst_cweights_frames > 150;
}

void zox_tst_chest_weights_check(ecs* world) {
    const entity skeleton = get_skeleton(world, zox_tst_cweights_npc);
    const entity mesh = zox_valid(skeleton) ? zox_get_link(world, skeleton, ActiveMesh) : 0;
    zox_tst_check(zox_valid(mesh) && zox_has(mesh, BoneIndexes), "the humanoid has no weighted skeleton mesh");
    if (!zox_valid(mesh) || !zox_has(mesh, BoneIndexes)) {
        return;
    }
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    const MeshVertices* vertices = zox_get(mesh, MeshVertices);
    const BoneIndexes* weights = zox_get(mesh, BoneIndexes);
    // the chest: the first bone, the body's core, the rest hang off it
    int inside_chest_only = 0, wrong = 0, shared_other = 0, deeper_in_chest = 0;
    for (int i = 0; i < vertices->length && i < weights->length; i++) {
        const float3 p = vertices->value[i];
        byte inside[bones->length];
        for (int k = 0; k < bones->length; k++) {
            const float3 c = zox_getv(bones->value[k], BonePosition);
            const float3 s = zox_getv(bones->value[k], BoneSize);
            inside[k] = fabsf(p.x - c.x) <= s.x && fabsf(p.y - c.y) <= s.y && fabsf(p.z - c.z) <= s.z;
        }
        byte only_chest = inside[0];
        for (int k = 1; k < bones->length && only_chest; k++) {
            only_chest = !inside[k];
        }
        if (only_chest) {
            inside_chest_only++;
            wrong += weights->value[i] != 0;
        } else if (inside[0] && weights->value[i] != 0) {
            // in the chest and another bone, moved by the other: how deep in each (0 its middle, 1 its edge)
            const int k = weights->value[i];
            const float3 cc = zox_getv(bones->value[0], BonePosition), cs = zox_getv(bones->value[0], BoneSize);
            const float3 oc = zox_getv(bones->value[k], BonePosition), os = zox_getv(bones->value[k], BoneSize);
            const float chest_depth = fmaxf(fabsf(p.x - cc.x) / cs.x, fmaxf(fabsf(p.y - cc.y) / cs.y, fabsf(p.z - cc.z) / cs.z));
            const float other_depth = fmaxf(fabsf(p.x - oc.x) / os.x, fmaxf(fabsf(p.y - oc.y) / os.y, fabsf(p.z - oc.z) / os.z));
            shared_other++;
            deeper_in_chest += chest_depth < other_depth;
        }
    }
    zox_log("[tst] [metric] chest_weights [%i] bones, [%i] vertices only in the chest, [%i] weighted to another bone",
        bones->length, inside_chest_only, wrong);
    zox_log("[tst] [metric] chest_weights [%i] vertices in the chest and another bone moved by the other, [%i] deeper in the chest",
        shared_other, deeper_in_chest);
    zox_tst_check(inside_chest_only > 0, "no vertices only in the chest");
    zox_tst_check(!deeper_in_chest, "[%i] of the chest's vertices, deeper in it, move with another bone", deeper_in_chest);
    zox_tst_check(!wrong, "[%i] of the chest\x27s vertices move with another bone", wrong);
}

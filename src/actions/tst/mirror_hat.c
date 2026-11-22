// tst: mirror_hat, the mirror (the character window) shows the player as they are: its hat changed, the mirror's too
//  (rendering3/fun/clones.c mesh_clones_move, bones skeleton_meshes.c swap_skeleton_mesh)
//  - the mirror opened (huds/dbg/mirror.c), the player's hat taken off (its slot emptied, the body rebuilt)
//  - once its skeleton swaps in the rebuilt mesh: the mirror's copy follows it (linked to it), its data the same
//  - was: the copy stayed linked to the old mesh, the old hat
//  - run: bash bsh/tst.sh mirror_hat
byte zox_tst_mhat_stage = 0;
int zox_tst_mhat_frames = 0;
entity zox_tst_mhat_old = 0;
entity zox_tst_mhat_clone = 0;
byte zox_tst_mhat_linked = 0, zox_tst_mhat_same = 0;

// the mirror's copy of a mesh: a clone named character_clone
static entity zox_tst_mhat_find_clone(ecs* world) {
    entity found = 0;
    iter it = ecs_each_id(world, ecs_pair(zox_id(MeshCloneLink), EcsWildcard));
    while (zox_each_next(it)) {
        for (int j = 0; j < it.count; j++) {
            if (!strncmp(zox_get_name(it.entities[j]), "character_clone", 15)) {
                found = it.entities[j];
            }
        }
    }
    return found;
}

byte zox_tst_mirror_hat_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    const entity active = zox_valid(skeleton) ? zox_get_link(world, skeleton, ActiveMesh) : 0;
    zox_tst_mhat_frames++;
    zox_tst_progress(zox_tst_mhat_stage * 1000 + zox_tst_mhat_frames);
    if (zox_tst_mhat_stage == 0) {
        zox_tst_waiting = "the player's skeleton mesh";
        if (!zox_valid(active) || !zox_get(active, MeshIndicies)->length || zox_tst_mhat_frames < 30) {
            return 0;
        }
        zox_tst_render_texture_character(world, dbg_player);
        zox_tst_mhat_stage = 1;
        zox_tst_mhat_frames = 0;
        return 0;
    }
    if (zox_tst_mhat_stage == 1) {
        zox_tst_waiting = "the mirror's copy, then the hat off";
        zox_tst_mhat_clone = zox_tst_mhat_find_clone(world);
        if (!zox_tst_mhat_clone) {
            return zox_tst_mhat_frames > 300;
        }
        const entity body = zox_get_child_by_id(world, character, zox_id(Body));
        entity slots[8];
        const uint count = get_equip_slots_in_children(world, body, zox_id(HatSlot), slots, 8);
        const entity hat = count ? zox_get_link(world, slots[0], DataLink) : 0;
        zox_tst_check(zox_valid(hat), "the player has no hat");
        if (!zox_valid(hat)) {
            return 1;
        }
        zox_unlink(world, slots[0], DataLink, hat);
        zox_setv(character, BodyDirty, zox_generate_body_start);
        zox_tst_mhat_old = active;
        zox_tst_mhat_stage = 2;
        zox_tst_mhat_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the rebuilt mesh swapped in, the mirror's copy following it";
    if (active == zox_tst_mhat_old || zox_tst_mhat_frames < 30) {
        return zox_tst_mhat_frames > 600;
    }
    const entity clone = zox_tst_mhat_clone;
    zox_tst_mhat_linked = zox_valid(clone) && zox_is_linked(world, clone, MeshCloneLink, active);
    zox_tst_mhat_same = zox_valid(clone) && zox_get(clone, MeshVertices)->length == zox_get(active, MeshVertices)->length &&
        zox_get(clone, MeshIndicies)->length == zox_get(active, MeshIndicies)->length;
    zox_log("[tst] mirror_hat: copy verts [%i] the mesh's [%i]", zox_valid(clone) ? zox_get(clone, MeshVertices)->length : -1,
        zox_get(active, MeshVertices)->length);
    zox_tst_mhat_stage = 3;
    return 1;
}

void zox_tst_mirror_hat_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_mhat_stage == 3, "stopped at stage [%i]", zox_tst_mhat_stage);
    zox_tst_check(zox_tst_mhat_linked, "the mirror's copy isn't linked to the rebuilt mesh");
    zox_tst_check(zox_tst_mhat_same, "the mirror's copy isn't the rebuilt mesh (the old hat)");
}

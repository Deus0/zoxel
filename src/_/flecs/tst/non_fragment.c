// tst: nf_*, non fragmenting parents (zox_set_parent_non_fragment, flecs EcsParent) when deleting
//  - ChildOf gives every parent's children their own tables (lag spikes streaming npcs, tst tables_stream),
//      non fragmenting parents don't, but deleting them crashed the game (end game, deleting a realm):
//      flecs asserted a non fragmenting child's parent was alive, deleting it in a cascade
//  - each case is its own test, a crash fails only it:
//  - nf_cascade: root -> middle (ChildOf) -> leaf (non fragmenting), delete the root
//  - nf_cascade_deferred: the same, deleted in a deferred batch (as systems do, merged later)
//  - nf_mixed_deferred: the middle has a ChildOf child and a non fragmenting child, deferred
//  - nf_all_deferred: non fragmenting all the way down, deferred
//  - nf_prefabs_deferred: like a realm of characters with stats, roots' middles (ChildOf, prefab instances)
//      each with non fragmenting leaves (prefab instances) that have children too, deferred
//  - each: every entity is deleted with the root
//  - run: bash bsh/tst.sh nf_cascade_deferred
static inline byte zox_tst_nf_ready(ecs* world) {
    (void) world;
    return 1;
}

// builds root -> middle -> leaf (and leaf2), parents' kinds by flags, deletes the root, checks all died
static inline void zox_tst_nf_case(ecs* world, byte middle_nf, byte leaf_nf, byte mixed, byte deferred) {
    const entity root = zox_new();
    const entity middle = zox_new();
    const entity leaf = zox_new();
    const entity leaf2 = mixed ? zox_new() : 0;
    if (middle_nf) {
        zox_set_parent_non_fragment(world, middle, root);
    } else {
        zox_set_parent(world, middle, root);
    }
    if (leaf_nf) {
        zox_set_parent_non_fragment(world, leaf, middle);
    } else {
        zox_set_parent(world, leaf, middle);
    }
    if (leaf2) {
        // the other kind
        if (leaf_nf) {
            zox_set_parent(world, leaf2, middle);
        } else {
            zox_set_parent_non_fragment(world, leaf2, middle);
        }
    }
    zox_tst_check(zox_get_parent(world, leaf) == middle && zox_get_parent(world, middle) == root, "parents not set");
    if (deferred) {
        // like a system: the delete is queued, then merged
        zox_defer_begin();
        zox_delete(root);
        zox_defer_end();
    } else {
        zox_delete(root);
    }
    zox_tst_check(!zox_alive(root), "the root is alive");
    zox_tst_check(!zox_alive(middle), "the middle wasn't deleted with the root");
    zox_tst_check(!zox_alive(leaf), "the leaf wasn't deleted with the root");
    zox_tst_check(!leaf2 || !zox_alive(leaf2), "the second leaf wasn't deleted with the root");
}

void zox_tst_nf_cascade_check(ecs* world) {
    zox_tst_nf_case(world, 0, 1, 0, 0);
}

void zox_tst_nf_cascade_deferred_check(ecs* world) {
    zox_tst_nf_case(world, 0, 1, 0, 1);
}

void zox_tst_nf_mixed_deferred_check(ecs* world) {
    zox_tst_nf_case(world, 0, 1, 1, 1);
}

void zox_tst_nf_all_deferred_check(ecs* world) {
    zox_tst_nf_case(world, 1, 1, 0, 1);
}

void zox_tst_nf_prefabs_deferred_check(ecs* world) {
    // prefabs, like characters' and stats'
    const entity prefab_middle = zox_new();
    zox_make_prefab(prefab_middle);
    const entity prefab_leaf = zox_new();
    zox_make_prefab(prefab_leaf);
    const entity root = zox_new();
    entity middles[6], leaves[6][5], kids[6][5];
    for (int m = 0; m < 6; m++) {
        middles[m] = ecs_new_w_pair(world, EcsIsA, prefab_middle);
        zox_set_parent(world, middles[m], root);
        for (int l = 0; l < 5; l++) {
            leaves[m][l] = ecs_new_w_pair(world, EcsIsA, prefab_leaf);
            zox_set_parent_non_fragment(world, leaves[m][l], middles[m]);
            kids[m][l] = zox_new();
            zox_set_parent_non_fragment(world, kids[m][l], leaves[m][l]);
        }
    }
    {
        zox_defer_begin();
        zox_delete(root);
        zox_defer_end();
    }
    int alive = 0;
    for (int m = 0; m < 6; m++) {
        alive += zox_alive(middles[m]);
        for (int l = 0; l < 5; l++) {
            alive += zox_alive(leaves[m][l]) + zox_alive(kids[m][l]);
        }
    }
    zox_tst_check(alive == 0, "[%i] entities survived deleting their root", alive);
    zox_delete(prefab_middle);
    zox_delete(prefab_leaf);
}

// like a realm's stats: stat prefabs ChildOf the realm, a character ChildOf the realm, the character's stats
//  instances of the prefabs, named, non fragmenting children of the character, deleting the realm
//  - spawn_deferred: the stats made and parented in one deferred batch (a spawn system)
static inline void zox_tst_nf_realm_case(ecs* world, byte spawn_deferred) {
    const entity realm = zox_new();
    const entity prefab_stat = zox_new();
    zox_set_parent(world, prefab_stat, realm);
    zox_make_prefab(prefab_stat);
    zox_set_name(prefab_stat, "tst_stat_prefab");
    const entity character = zox_new();
    zox_set_parent(world, character, realm);
    entity stat = 0;
    if (spawn_deferred) {
        zox_defer_begin();
        stat = zox_ins_named(world, prefab_stat);
        zox_set_parent_non_fragment(world, stat, character);
        zox_defer_end();
    } else {
        stat = zox_ins_named(world, prefab_stat);
        zox_set_parent_non_fragment(world, stat, character);
    }
    char* type = ecs_type_str(world, ecs_get_type(world, stat));
    zox_log("[tst] the stat's type: %s", type);
    ecs_os_free(type);
    zox_tst_check(zox_get_parent(world, stat) == character, "the stat's parent isn't its character");
    {
        zox_defer_begin();
        zox_delete(realm);
        zox_defer_end();
    }
    zox_tst_check(!zox_alive(character) && !zox_alive(stat) && !zox_alive(prefab_stat), "the realm's entities survived it");
}

void zox_tst_nf_realm_check(ecs* world) {
    zox_tst_nf_realm_case(world, 0);
}

void zox_tst_nf_realm_spawn_deferred_check(ecs* world) {
    zox_tst_nf_realm_case(world, 1);
}

// like a realm of npcs: several characters (ChildOf the realm), each with several non fragmenting stats
//  (instances of the realm's stat prefabs), the stats of every character share tables (non fragmenting),
//  deleting the realm, deferred
//  - flecs deletes the marked tables in an order, if the shared stats table comes after the characters',
//      every stat's parent is dead already (the game crashed ending a game)
void zox_tst_nf_realm_many_check(ecs* world) {
    const entity realm = zox_new();
    entity prefabs[4];
    for (int p = 0; p < 4; p++) {
        prefabs[p] = zox_new();
        zox_set_parent(world, prefabs[p], realm);
        zox_make_prefab(prefabs[p]);
    }
    entity characters[8], stats[8][4];
    for (int c = 0; c < 8; c++) {
        characters[c] = zox_new();
        zox_set_parent(world, characters[c], realm);
    }
    // stats spawned after their characters, in a batch, like a spawn system
    {
        zox_defer_begin();
        for (int c = 0; c < 8; c++) {
            for (int p = 0; p < 4; p++) {
                stats[c][p] = zox_ins(world, prefabs[p]);
                zox_set_parent_non_fragment(world, stats[c][p], characters[c]);
            }
        }
        zox_defer_end();
    }
    {
        zox_defer_begin();
        zox_delete(realm);
        zox_defer_end();
    }
    int alive = 0;
    for (int c = 0; c < 8; c++) {
        alive += zox_alive(characters[c]);
        for (int p = 0; p < 4; p++) {
            alive += zox_alive(stats[c][p]);
        }
    }
    zox_tst_check(alive == 0, "[%i] entities survived deleting their realm", alive);
}

// everything the game's realm has: characters as prefab instances, linked to the realm (TstLink), with ChildOf
//  children (body parts) and non fragmenting stats, stats instances of prefabs of a base prefab, linked
//  to each other (TstLinks), stat prefabs ChildOf the realm, a 3D ui owned by each character (TstOwnedLink)
void zox_tst_nf_realm_full_check(ecs* world) {
    const entity realm = zox_new();
    const entity base_stat = zox_new();
    zox_make_prefab(base_stat);
    const entity base_character = zox_new();
    zox_make_prefab(base_character);
    entity prefabs[4];
    for (int p = 0; p < 4; p++) {
        prefabs[p] = zox_prefab_from_parent(world, base_stat);
        zox_set_parent(world, prefabs[p], realm);
        if (p > 0) {
            zox_link(world, prefabs[p], TstLink, prefabs[0]);
        }
    }
    entity characters[8], stats[8][4], parts[8], uis[8];
    for (int c = 0; c < 8; c++) {
        characters[c] = zox_ins(world, base_character);
        zox_set_parent(world, characters[c], realm);
        zox_link(world, characters[c], TstLinks, realm);
        parts[c] = zox_new();
        zox_set_parent(world, parts[c], characters[c]);
        uis[c] = zox_new();
        zox_link(world, uis[c], TstOwnedLink, characters[c]);
    }
    {
        zox_defer_begin();
        for (int c = 0; c < 8; c++) {
            for (int p = 0; p < 4; p++) {
                stats[c][p] = zox_ins_named(world, prefabs[p]);
                zox_set_parent_non_fragment(world, stats[c][p], characters[c]);
            }
        }
        zox_defer_end();
    }
    {
        zox_defer_begin();
        zox_delete(realm);
        zox_defer_end();
    }
    int alive = 0;
    for (int c = 0; c < 8; c++) {
        alive += zox_alive(characters[c]) + zox_alive(parts[c]) + zox_alive(uis[c]);
        for (int p = 0; p < 4; p++) {
            alive += zox_alive(stats[c][p]);
        }
    }
    zox_tst_check(alive == 0, "[%i] entities survived deleting their realm", alive);
    zox_delete(base_stat);
    zox_delete(base_character);
}

// tst: nf_dying_parent, a child parented in the same deferred batch its parent is deleted in (a spawn system's
//  child into a chunk the game's end deletes that frame): the child goes with it, not left alive with no parent
//  - each way: ChildOf and non fragmenting, the parent set before the delete and after it (stages merge in any order)
//  - flecs patch "dying parent" (doc/engine/flecs_patches.md)
//  - run: bash bsh/tst.sh nf_dying_parent
void zox_tst_nf_dying_parent_check(ecs* world) {
    for (byte nf = 0; nf <= 1; nf++) {
        for (byte delete_first = 0; delete_first <= 1; delete_first++) {
            const entity parent = zox_new();
            zox_defer_begin();
            const entity child = zox_new();
            if (delete_first) {
                zox_delete(parent);
            }
            if (nf) {
                zox_set_parent_non_fragment(world, child, parent);
            } else {
                zox_set_parent(world, child, parent);
            }
            if (!delete_first) {
                zox_delete(parent);
            }
            zox_defer_end();
            zox_tst_check(!zox_alive(parent), "the parent is alive");
            zox_tst_check(!zox_alive(child), "[%s, parent set %s its delete] the child outlived its parent",
                nf ? "non fragmenting" : "ChildOf", delete_first ? "after" : "before");
            if (zox_alive(child)) {
                zox_delete(child);
            }
        }
    }
}

// tst: nf_dying_parent_stage, as nf_dying_parent, the child spawned in another stage (a multithreaded system's queue)
//  merged after the parent's delete was applied: its parent set to a dead entity at the merge
void zox_tst_nf_dying_parent_stage_check(ecs* world) {
    for (byte nf = 0; nf <= 1; nf++) {
        const entity parent = zox_new();
        ecs_world_t* stage = ecs_stage_new(world);
        const entity child = ecs_new(stage);
        if (nf) {
            const EcsParent value = { parent };
            ecs_set_id(stage, child, ecs_id(EcsParent), sizeof(EcsParent), &value);
        } else {
            ecs_add_pair(stage, child, EcsChildOf, parent);
        }
        zox_delete(parent);
        ecs_merge(stage);
        ecs_stage_free(stage);
        zox_tst_check(!zox_alive(parent), "the parent is alive");
        zox_tst_check(!zox_alive(child), "[%s] the child outlived its parent, its parent set after its delete merged",
            nf ? "non fragmenting" : "ChildOf");
        if (zox_alive(child)) {
            zox_log("[tst] nf_dying_parent_stage: the orphan's type: %s", ecs_type_str(world, ecs_get_type(world, child)));
            zox_delete(child);
        }
    }
}

// tst: nf_dying_grandparent, as terrain streaming spawns: a realm -> terrain (ChildOf), chunks named prefab instances,
//  non fragmenting children of the terrain, tunks ChildOf it, spawned in the deferred batch the realm is deleted in
//  - each order: the realm's delete queued before the spawns and after
void zox_tst_nf_dying_grandparent_check(ecs* world) {
    const entity prefab = zox_new();
    zox_make_prefab(prefab);
    zox_set_name(prefab, "tst_chunk_prefab");
    for (byte named = 0; named <= 1; named++)
    for (byte delete_first = 0; delete_first <= 1; delete_first++) {
        const entity realm = zox_new();
        const entity terrain = zox_new();
        zox_set_parent(world, terrain, realm);
        zox_defer_begin();
        if (delete_first) {
            zox_delete(realm);
        }
        const entity chunk = zox_ins(world, prefab);
        if (named) {
            zox_set_unique_name(chunk, "tst_chunk_0x0x0");
        }
        zox_set_parent_non_fragment(world, chunk, terrain);
        const entity tunk = zox_ins(world, prefab);
        if (named) {
            zox_set_unique_name(tunk, "tst_tunk_0x0");
        }
        zox_set_parent(world, tunk, terrain);
        if (!delete_first) {
            zox_delete(realm);
        }
        zox_defer_end();
        zox_tst_check(!zox_alive(terrain), "the terrain is alive");
        zox_tst_check(!zox_alive(chunk), "[delete %s, %s] the chunk (non fragmenting) outlived its terrain",
            delete_first ? "first" : "last", named ? "named" : "unnamed");
        zox_tst_check(!zox_alive(tunk), "[delete %s, %s] the tunk (ChildOf) outlived its terrain",
            delete_first ? "first" : "last", named ? "named" : "unnamed");
        if (zox_alive(chunk)) {
            zox_log("[tst] orphan chunk: %s", ecs_type_str(world, ecs_get_type(world, chunk)));
            zox_delete(chunk);
        }
        if (zox_alive(tunk)) {
            zox_log("[tst] orphan tunk: %s", ecs_type_str(world, ecs_get_type(world, tunk)));
            zox_delete(tunk);
        }
    }
    zox_delete(prefab);
}

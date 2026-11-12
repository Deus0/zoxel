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

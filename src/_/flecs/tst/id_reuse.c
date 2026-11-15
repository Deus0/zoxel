// tst: instance_id_reuse, a deleted entity's handle stays dead when a prefab is instanced
//  - flecs gives an instance's children ids at their prefab's offsets (instance + (child - prefab)); when that id
//      had been a deleted entity's it came back alive with the instance's generation, not a newer one: the deleted
//      entity's old handle could be alive again, as the new child
//  - in game: a deleted chunk's id became an npc's stat, its neighbours' links to it read alive (a segfault in
//      chunk_sides_system, the lights before it), doc/todo.md (Bugs), doc/flecs_patches.md (instance child ids)
//  - run: bash bsh/tst.sh instance_id_reuse
byte zox_tst_instance_id_reuse_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_instance_id_reuse_check(ecs* world) {
    const entity prefab = zox_new();
    zox_make_prefab(prefab);
    // a child with an id above its prefab's (recycled ids can come lower)
    entity child = 0;
    for (int i = 0; i < 64 && (uint) child <= (uint) prefab; i++) {
        child = zox_new();
    }
    zox_make_prefab(child);
    zox_set_parent(world, child, prefab);
    const uint offset = (uint) child - (uint) prefab;
    // the instance to be, and the entity at the id its child will want
    const entity instance = zox_new();
    entity victim = 0;
    for (int i = 0; i < 256 && !victim; i++) {
        const entity e = zox_new();
        if ((uint) e == (uint) instance + offset) {
            victim = e;
        }
    }
    zox_tst_check(victim, "no entity came at the instance's id + [%u], the test couldn't set up", offset);
    if (!victim) {
        return;
    }
    zox_delete(victim);
    zox_tst_check(!zox_valid(victim), "the deleted entity is still valid");
    ecs_add_pair(world, instance, EcsIsA, prefab);
    const entity copy = zox_get_child_by_index(world, instance, 0);
    zox_log("[tst] deleted [%u] gen [%u], the instance [%u] gen [%u], its child [%u] gen [%u]", (uint) victim,
        (uint) (victim >> 32), (uint) instance, (uint) (instance >> 32), (uint) copy, (uint) (copy >> 32));
    zox_tst_check(zox_valid(copy), "the instance got no child");
    zox_tst_check(!zox_valid(victim), "the deleted entity's handle is alive again, as the instance's child");
    // its id reused is fine, with a newer generation
    zox_tst_check((uint) copy != (uint) victim || (copy >> 32) > (victim >> 32),
        "the instance's child took the deleted entity's id with generation [%u], not above its [%u]",
        (uint) (copy >> 32), (uint) (victim >> 32));
}

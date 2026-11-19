// tst: flecs_wildcard_remove, a flecs bug: removing (R, *) from a prefab's instance asserts, when R has a size (a
//  zox_on_unlink link: a 1 byte type with an on_remove hook, as DataLink)
//  - flecs.c ecs_table_get_column_index: assert(ecs_id_is_valid(world, id)), from flecs_get_table_override, from
//      flecs_emit_on_set_for_override_on_remove (removing a component, it looks for the prefab's value to set back,
//      the wildcard as its id)
//  - the prefab has a component (with none it was fine); deferred or not; a plain entity, or removing (R, target), is
//      fine
//  - found as a gamepad picked up an item: its slot's DataLink cleared, zox_set_link(slot, DataLink, 0), crashed
//  - zox_set_link (src/_/flecs/mcr/pair.c) removes each target instead, this checks it still has to
//  - each case runs in a forked child (the assert aborts it), the parent reads how it ended
//  - fails once flecs fixes it: then zox_set_link can remove (T, *) again
//  - run: bash bsh/tst.sh flecs_wildcard_remove
#ifndef zox_windows
    #include <sys/wait.h>
#endif
zox_tag(TstWildLink);
zoxc_byte(TstWildValue);

void on_unlink_TstWildLink(ecs* world, entity e, entity target) {
    (void) world; (void) e; (void) target;
}

void zox_components_tst_wildcard_remove(ecs* world) {
    zoxd_nf_link(TstWildLink);
    zox_on_unlink(on_unlink_TstWildLink, TstWildLink);
    zoxd_byte(TstWildValue);
}

// a prefab with a component (a prefab with none was fine), its instance
static inline entity zox_tst_wild_instance(ecs* world) {
    const entity prefab = ecs_new_w_id(world, EcsPrefab);
    zox_setv(prefab, TstWildValue, 1);
    return ecs_new_w_pair(world, EcsIsA, prefab);
}

// e (a prefab's instance, or a plain entity) linked to a target, then (R, *) or (R, target) removed; 1 if it ran
static byte zox_tst_wild_case(ecs* world, byte instance, byte wildcard) {
#ifdef zox_windows
    (void) world; (void) instance;
    return !(instance && wildcard);     // posix only (fork), as found on linux
#else
    fflush(stdout);
    const pid_t pid = fork();
    if (pid == 0) {
        const entity target = zox_new();
        const entity e = instance ? zox_tst_wild_instance(world) : zox_new();
        zox_link(world, e, TstWildLink, target);
        ecs_remove_pair(world, e, zox_id(TstWildLink), wildcard ? EcsWildcard : target);
        _exit(zox_is_linked_any(world, e, TstWildLink) ? 2 : 0);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    const byte ran = WIFEXITED(status) && WEXITSTATUS(status) == 0;
    zox_log("[tst] %s, removing %s: [%s]", instance ? "a prefab's instance" : "a plain entity",
        wildcard ? "(R, *)" : "(R, target)", ran ? "ran" : WIFSIGNALED(status) ? "aborted" : "still linked");
    return ran;
#endif
}

byte zox_tst_wildcard_remove_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_wildcard_remove_check(ecs* world) {
    zox_tst_check(zox_tst_wild_case(world, 0, 0), "removing (R, target) from a plain entity failed");
    zox_tst_check(zox_tst_wild_case(world, 1, 0), "removing (R, target) from a prefab's instance failed");
    zox_tst_check(zox_tst_wild_case(world, 0, 1), "removing (R, *) from a plain entity failed");
    // the bug
    zox_tst_check(!zox_tst_wild_case(world, 1, 1), "flecs fixed removing (R, *) from a prefab's instance: zox_set_link "
        "can remove (T, *) again (src/_/flecs/mcr/pair.c), and this test can go");
    // the way round it: zox_set_link to none on a prefab's instance
    const entity e = zox_tst_wild_instance(world);
    zox_link(world, e, TstWildLink, zox_new());
    zox_set_link(world, e, TstWildLink, 0);
    zox_tst_check(!zox_is_linked_any(world, e, TstWildLink), "zox_set_link(e, T, 0) left a link");
}

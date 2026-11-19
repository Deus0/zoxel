// Npcs glance at players: a humanoid npc's head (a skeleton with a head bone) looks at the player it sees (its
//  Target, sys/sight.c), and ahead again once it doesn't (bones/sys/head_look.c turns it)
//  - not while following someone (FollowTarget, a dialogue: its speakers look at each other)
//  - boneless npcs (slimes) don't, see the todo (tumble towards the player)
void npc_glance_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Target, sight);
        // following someone (its FollowTarget, set by a dialogue): its speakers look at each other
        if (zox_valid(zox_get_link(world, e, FollowTarget))) {
            continue;
        }
        const entity skeleton = get_skeleton(world, e);
        const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
        if (!zox_valid(head)) {
            continue;
        }
        const entity look = zox_valid(sight->value) ? sight->value : 0;
        if (zox_getv(head, LookTarget) != look) {
            zox_setv(head, LookTarget, look);
        }
    }
    zox_sys_on_end();
} zoxd_system(npc_glance_system);

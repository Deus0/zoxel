// Npcs glance at players walking by: a humanoid npc's head (a skeleton with a head bone) looks at a player in its
//  chunk within reach, and ahead again once they leave (bones/sys/head_look.c turns it)
//  - not while following someone (a dialogue, its speakers look at each other)
//  - its chunk's ChunkEntities, only changed in zoxp_update (chunk_link_system), read here on workers
//  - boneless npcs (slimes) don't, see the todo (tumble towards the player)
const float npc_glance_distance = 3;

void npc_glance_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Behaviour);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(Behaviour, behaviour);
        if (behaviour->value == zox_behaviour_follow) {
            continue;
        }
        const entity skeleton = get_skeleton(world, e);
        const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
        const entity chunk = zox_get_link(world, e, ChunkLink);
        if (!zox_valid(head) || !zox_valid(chunk) || !zox_has(chunk, ChunkEntities)) {
            continue;
        }
        entity look = 0;
        float closest = npc_glance_distance;
        const ChunkEntities* entities = zox_get(chunk, ChunkEntities);
        for (int j = 0; j < entities->length; j++) {
            const entity e2 = entities->value[j];
            if (e2 == e || !zox_valid(e2) || !zox_valid(zox_get_link(world, e2, PlayerLink))) {
                continue;
            }
            const float distance = float3_distance(position->value, zox_getv(e2, Position3D));
            if (distance < closest) {
                closest = distance;
                look = e2;
            }
        }
        if (zox_getv(head, LookTarget) != look) {
            zox_setv(head, LookTarget, look);
        }
    }
    zox_sys_on_end();
} zoxd_system(npc_glance_system);

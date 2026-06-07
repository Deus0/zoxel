// TODO: Make this use bytes for collision
zox_sys2(SphereCollideSystem) {
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(SphereRadius);
    zox_sys_in(CollisionDisabled);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        zox_sys_i(SphereRadius, radius);
        zox_sys_i(CollisionDisabled, disabled);
        if (disabled->value) {
            continue;
        }
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(Position3D);
            zox_sys_in_2(SphereRadius);
            zox_sys_in_2(CollisionDisabled);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(Position3D, position2);
                zox_sys_i_2(SphereRadius, radius2);
                zox_sys_i_2(CollisionDisabled, disabled2);
                if (disabled2->value) {
                    continue;
                }
                entity e2 = it2.entities[j];
                if (e == e2) {
                    continue;
                }
                float distance = float3_distance(position->value, position2->value);
                byte overlaps = (distance <= radius->value + radius2->value);
                if (overlaps) {
                    if (zox_has(e, OverlapEvent)) {
                        zox_geter(e, OverlapEvent, oevent);
                        if (oevent->value) {
                            (*oevent->value)(world, e, e2);
                        }
                    }
                    // zox_log(" > e [%lu] overlapping e2 [%lu]\n", e, e2)
                    // detects when one object overlaps another
                    // we should do overlap event here
                    // that would involve caching overlapped entities tho
                }
            }
        }
        zox_sys_query_end();
    }
} zox_sys_end(SphereCollideSystem);

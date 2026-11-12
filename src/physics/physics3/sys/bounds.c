//! Bounds, because everyone needs limits! Used atm to keep player within the start area.
void Position3DBoundsSystem(iter* it) {
    zox_sys_on_begin();
    const float teleport_force = 0.2f;
    zox_physics_profile_begin();
    zox_sys_begin();
    zox_sys_in(Position3DBounds);
    zox_sys_in(Bounds3D);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3DBounds, position3DBounds);
        zox_sys_i(Bounds3D, bounds3D);
        zox_sys_o(Position3D, position3D);
        const float6 bounds = position3DBounds->value;
        if (bounds.x == 0 && bounds.y == 0 && bounds.z == 0 && bounds.w == 0 && bounds.u == 0 && bounds.v == 0) {
            continue; // disabled
        }
        // the whole box stays inside, pushed teleport_force past the edge it crossed
        const float3 extents = bounds3D->value;
        float3* p = &position3D->value;
        if (p->x - extents.x < bounds.x) p->x = bounds.x + extents.x + teleport_force;
        else if (p->x + extents.x > bounds.y) p->x = bounds.y - extents.x - teleport_force;
        if (p->y - extents.y < bounds.z) p->y = bounds.z + extents.y + teleport_force;
        else if (p->y + extents.y > bounds.w) p->y = bounds.w - extents.y - teleport_force;
        if (p->z - extents.z < bounds.u) p->z = bounds.u + extents.z + teleport_force;
        else if (p->z + extents.z > bounds.v) p->z = bounds.v - extents.z - teleport_force;
    }
    zox_physics_profile_end();
    zox_sys_on_end();
} zoxd_system(Position3DBoundsSystem);

//! Each entity will impact all other entities.
void Gravity2DSystem(iter *it) {
    // NearbyEntities component instead of everything.
    double delta_time = (double) it->delta_time;
    Position2 *position2s = ecs_field(it, Position2, 1);
    Velocity2D *velocity2Ds = ecs_field(it, Velocity2D, 2);
    for (int i = 0; i < it->count; i++)
    {
        const Velocity2D *velocity2D = &velocity2Ds[i];
        Position2 *position2 = &position2s[i];
        position2->value.x += velocity2D->value.x * delta_time;
        position2->value.y += velocity2D->value.y * delta_time;
    }
}

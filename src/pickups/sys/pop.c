// Animates a dropped item's pop: an arc from where it dropped to a spot nearby
//  - horizontally it eases out, vertically it hops (a parabola), then it rests at the end
//  - stops if it's picked up mid pop
void pickup_pop_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PickupPopStart);
    zox_sys_in(PickupPopEnd);
    zox_sys_in(PickupPopHeight);
    zox_sys_in(PickupPopTime);
    zox_sys_in(PickedUp);
    zox_sys_out(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PickupPopStart, start);
        zox_sys_i(PickupPopEnd, end);
        zox_sys_i(PickupPopHeight, height);
        zox_sys_i(PickupPopTime, time);
        zox_sys_i(PickedUp, picked);
        zox_sys_o(Position3D, position);
        if (picked->value != pickup_state_none) {
            zox_remove(e, PickupPop);
            continue;
        }
        const float t = (float) ((zox_current_time - time->value) / pickup_pop_seconds);
        if (t >= 1) {
            position->value = end->value;
            zox_remove(e, PickupPop);
            continue;
        }
        const float eased = 1 - (1 - t) * (1 - t);
        position->value = float3_lerp(start->value, end->value, eased);
        position->value.y += height->value * 4 * t * (1 - t);
    }
    zox_sys_on_end();
} zoxd_system(pickup_pop_system);

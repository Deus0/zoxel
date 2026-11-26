void unfold_toggle(ecs* world, entity owner, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    if (zox_is_linked(world, owner, Unfolded, e)) {
        zox_unlink(world, owner, Unfolded, e);
    } else {
        zox_link(world, owner, Unfolded, e);
    }
    zox_add(owner, Dirty);
}

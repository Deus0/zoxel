byte unfolded_has(const Unfolded* unfolded, entity e) {
    for (int i = 0; unfolded && i < unfolded->length; i++) {
        if (unfolded->value[i] == e) {
            return 1;
        }
    }
    return 0;
}

void unfold_toggle(ecs* world, entity owner, entity e) {
    zox_muter(owner, Unfolded, unfolded);
    if (!remove_from_Unfolded(unfolded, e)) {
        add_to_Unfolded(unfolded, e);
    }
    zox_add(owner, Dirty);
}

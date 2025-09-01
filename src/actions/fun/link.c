

void set_linked_action(
    ecs *world,
    const entity user,
    const int index,
    const entity element
) {
    if (!zox_valid(user) || !zox_has(user, ActionLinks)) {
        return;
    }
    zox_mut_begin(user, ActionLinks, datas)
    if (index >= datas->length) {
        return;
    }
    datas->value[index] = element;
    zox_mut_end(user, ActionLinks);
}
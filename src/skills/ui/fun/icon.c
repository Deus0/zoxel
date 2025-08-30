void set_linked_skill(
    ecs *world,
    const entity user,
    const int index,
    const entity e
) {
    if (!zox_valid(user) || !zox_has(user, SkillLinks)) {
        return;
    }
    zox_mut_begin(user, SkillLinks, datas);
    if (index >= datas->length) {
        return;
    }
    datas->value[index] = e;
    zox_mut_end(user, SkillLinks);
}
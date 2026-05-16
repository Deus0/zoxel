void set_linked_skill(ecs* world, entity user, int index, entity e) {
    zox_set_parent(world, e, user);
    /*if (!zox_valid(user) || !zox_has(user, SkillLinks)) {
        return;
    }
    zox_mut_begin(user, SkillLinks, datas);
    if (index >= datas->length) {
        return;
    }
    datas->value[index] = e;
    zox_mut_end(user, SkillLinks);*/
}

entity spawn_user_quest(
    ecs* world,
    entity parent,
    entity prefab)
{
    entity e = zox_ins_named(world, prefab);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    return e;
}

entity spawn_quest_giving(
    ecs* world,
    entity parent,
    entity prefab)
{
    entity e = spawn_user_quest(
        world,
        parent,
        prefab);
    if (e) {
        zox_add(e, QuestGiving);
    }
    return e;
}

entity spawn_quest_doing(
    ecs* world,
    entity parent,
    entity prefab)
{
    entity e = spawn_user_quest(
        world,
        parent,
        prefab);
    if (e) {
        zox_add(e, QuestDoing);
    }
    return e;
}

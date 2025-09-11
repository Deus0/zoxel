void spawn_character_quests(
    ecs *world,
    spawned_character3D_data *data
) {
    if (!data->p) {
        return;
    }
    // get voxels
    zox_geter(data->realm, QuestLinks, realm_quests);
    if (!realm_quests->length) {
        return;
    }

    entity add_quest = realm_quests->value[0];
    if (!add_quest) {
        return;
    }

    QuestLinks quests = (QuestLinks) { 0 };

    {
        const entity quest = spawn_user_quest(
            world,
            add_quest,
            data->e
        );
        add_to_QuestLinks(&quests, quest);
    }

    zox_set_ptr(data->e, QuestLinks, quests);
}
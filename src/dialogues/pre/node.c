entity spawn_prefab_dialogue_node(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("speech_node");
    zox_add(e, SpeechNode);
    zox_set(e, DialogueText, { });
    return e;
}

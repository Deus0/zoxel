entity spawn_prefab_node_speech(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("speech_node");
    zox_add(e, SpeechNode);
    zox_set(e, DialogueText, { });
    return e;
}

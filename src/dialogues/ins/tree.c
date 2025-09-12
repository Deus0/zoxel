entity spawn_dialogue_tree(
    ecs* world,
    entity prefab,
    char* text
) {
    zox_instance(prefab);
    zox_name("dialogue_tree");

    DialogueText textc = { };
    size_t len = strlen(text);
    if (len >= 512) len = 512 - 1;
    memcpy(textc.value, text, len);
    textc.value[len] = '\0';
    zox_set_ptr(e, DialogueText, textc);
    zox_log("Testing Dialogue: [%s]", text);

    return e;
}
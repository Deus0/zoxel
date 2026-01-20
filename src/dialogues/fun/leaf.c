void set_node_speech(ecs* world, entity e, char* text) {
    /*DialogueText textc = { };
    size_t len = strlen(text);
    if (len >= 512) {
        len = 512 - 1;
    }
    memcpy(textc.value, text, len);
    textc.value[len] = '\0';
    zox_set_ptr(e, DialogueText, textc);*/
    // zox_log("Set Node: [%s]", text);
    set_DialogueText(world, e, text);
}
zox_tag(DialogueNode);
zox_tag(DialogueLeaf);
zox_tag(DialogueTree);
zoxc_fixed_string(DialogueText, 512);

void define_components_dialogues(ecs* world) {
    zoxd_tag(DialogueNode);
    zoxd_tag(DialogueLeaf);
    zoxd_tag(DialogueTree);
    zoxd_fixed_string(DialogueText);
}
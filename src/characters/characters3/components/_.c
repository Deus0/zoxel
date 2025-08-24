zox_tag(Character3);
zox_tag(SaveCharacter);
zoxc_byte(CharactersChanceMax);
zoxc_byte(Character3Type);
zoxc(CharacterSaveHash, ulong);
zoxc_entity(Character3Link);
zoxc_entity(Character3PrefabLink);

void define_components_characters3(ecs_world_t* world) {
    zoxd_tag(Character3);
    zoxd_tag(SaveCharacter);
    zoxd_byte(CharactersChanceMax);
    zoxd_byte(Character3Type);
    zox_define_component(CharacterSaveHash);
    zoxd_entity(Character3Link);
    zoxd_entity(Character3PrefabLink);
}
zox_tag(Character3);
zox_tag(SaveCharacter);
zoxc_byte(CharactersChanceMax);
zoxc_byte(Character3Type);
zoxc(CharacterSaveHash, ulong);
zoxc_entity(Character3Link);
zoxc_entity(Character3PrefabLink);
zoxc_entity(CharacterMetaLink);

void define_components_characters3(ecs* world) {
    zoxd_tag(Character3);
    zoxd_tag(SaveCharacter);
    zoxd_byte(CharactersChanceMax);
    zoxd_byte(Character3Type);
    zoxd(CharacterSaveHash);
    zoxd_entity(Character3Link);
    zoxd_entity(Character3PrefabLink);
    zoxd_entity(CharacterMetaLink);
}
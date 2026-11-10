zox_tag(Character);
zox_tag(CharacterUnique);
zoxc_state(GenerateCharacter);
zox_tag(CharacterLink);
#define zox_realm_characters_max 64
zox_tag(CharacterLinks);   // links to many character metas, i.e. from a realm
zox_tag(CharacterSlime);   // a slime character meta

void zox_components_characters(ecs* world) {
    zoxd_tag(Character);
    zoxd_tag(CharacterUnique);
    zoxd_state(GenerateCharacter);
    zoxd_nf_tag(CharacterLink);
    zoxd_nf_links(CharacterLinks);
    zoxd_tag(CharacterSlime);
}

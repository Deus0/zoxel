zox_tag(Realm);
zox_tag(RealmLink);
zoxc_byte(GenerateRealm);

void zox_components_realms(ecs* world) {
    zoxd_tag(Realm);
    zoxd_nf_link(RealmLink);
    zoxd_byte(GenerateRealm);
}
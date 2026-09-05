entity spawn_prefab_music(ecs *world) {
    zox_prefab();
    zox_prefab_name("music");
    zox_add(e, Music);
    zox_prefab_set(e, MusicLength, { 0 });
    zox_prefab_set(e, MusicTime, { 0 });
    zox_prefab_set(e, MusicSpeed, { 1.0 });
    zox_prefab_set(e, MusicNote, { 0 });
    zox_prefab_set(e, MusicEnabled, { 0 });
    zox_prefab_add(e, NoteLinks);
    return e;
}

entity spawn_music(
    ecs *world,
    entity prefab,
    double speed)
{
    zox_instance(prefab);
    zox_name("music");
    zox_set(e, MusicSpeed, { speed });
    return e;
}

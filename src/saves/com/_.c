zox_tag(Saver);
zoxc_state(SaveDirty);
zoxc_state(Saving);
zoxc_ulong(SaveHash);
zoxc_state(Loading);
zoxc_byte(Loaded);
zoxc_entity(FolderPathLink); // Link our entities to realm for folder path
zoxc_fixed_string(FolderPath, 512);
zoxc_fixed_string(SaveGamePath, 32); // TODO: Reduce this size for FilePaths
zoxc(FileLink, FILE*);

ECS_DTOR(FileLink, ptr, {
    if (ptr->value) {
        if (fclose(ptr->value)) {
            zox_loge("Failed to close file (FileLink)");
        }
    }
})

void zox_define_components_saves(ecs* world) {
    zoxd_tag(Saver);
    zoxd_state(SaveDirty);
    zoxd_state(Saving);
    zoxd_ulong(SaveHash);
    zoxd_state(Loading);
    zoxd_byte(Loaded);
    zoxd_entity(FolderPathLink);
    zoxd_fixed_string(FolderPath);
    zoxd_fixed_string(SaveGamePath);
    zoxd_dest(FileLink);
}

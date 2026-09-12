zox_tag(Saver);
// Events
zox_tag(Loaded);
zox_tag(Loading);
zox_tag(Edited);
zox_tag(SaveDirty);
zox_tag(Saving);
zoxc_ulong(SaveHash);
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
    // Events
    zoxd_nf_tag(Loaded);
    zoxd_tag(Loading);
    zoxd_tag(Edited);
    zoxd_tag(SaveDirty);
    zoxd_nf_tag(Saving);
    zoxd_ulong(SaveHash);
    zoxd_entity(FolderPathLink);
    zoxd_fixed_string(FolderPath);
    zoxd_fixed_string(SaveGamePath);
    zoxd_dest_old(FileLink);
}

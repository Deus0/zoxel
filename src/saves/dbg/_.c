extern entity get_linked_character(ecs*, entity);
extern entity get_linked_realm(ecs*, entity);

uint zox_dbg_ui_filepaths(ecs *world, entity e, char *buffer, uint size, uint index) {
    if (!e) {
        index += snprintf(buffer + index, size - index, "Invalid Player\n");
        return index;
    }
    index += snprintf(buffer + index, size - index, "Filepaths Debugger\n");
    entity game = zox_get_parent(world, e); // zox_getv(e, GameLink);
    entity realm = get_linked_realm(world, game);
    if (!zox_valid(realm)) {
        return index;
    }
    zox_geter(realm, FolderPath, folder_path);
    index += snprintf(buffer + index, size - index, "Realm Folder [%s]\n - [%s]\n", zox_get_name(realm), folder_path->value);
    entity character = get_linked_character(world, e);
    if (zox_valid(character)) {
        char* character_path = join_path(folder_path->value, "player.dat");
        index += snprintf(buffer + index, size - index, "Character [%s]\n - [%s]\n", zox_get_name(character), character_path);
        free(character_path);
    }
    return index;
}

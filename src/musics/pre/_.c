entity prefab_note;
entity prefab_music;
entity prefab_music_generated;
entity prefab_music_file;
entity prefab_playlist;
// link this to realm instead
entity local_music;

#include "note.c"
#include "music.c"
#include "music_generated.c"
#include "music_file.c"
#include "playlist.c"

void spawn_prefabs_musics(ecs *world) {
    prefab_note = spawn_prefab_note(world);
    prefab_music = spawn_prefab_music(world);
    prefab_music_generated = spawn_prefab_music_generated(
        world,
        prefab_music);
    prefab_music_file = spawn_prefab_music_file(
        world,
        prefab_music);
    prefab_playlist = spawn_prefab_playlist(world);
}

// todo: include all these automatically
#include "events/_.c"
#include "transforms/_.c"
#include "networking/_.c"
#include "triggers/_.c"
#include "screens/_.c"
#include "inputs/_.c"
#include "graphics/_.c"
#include "apps/_.c"
#include "saves/_.c"
// basic b locks
#include "assets/_.c" // uses sdl path function atm
#include "realms/_.c"
#include "games/_.c"
#include "players/_.c"
#include "colors/_.c"
#include "nodes/_.c"
#include "slots/_.c"
// beef
#include "cameras/_.c"
#include "rendering/_.c"
#include "sounds/_.c"
#include "raycasts/_.c"
#include "lines/_.c"
#include "shapes/_.c"
// inner core
#include "textures/_.c"
#include "cubes/_.c"
#include "musics/_.c"
#include "animations/_.c"
#include "bones/_.c"
#include "ui/_.c"
#include "lines/lines2/_.c"
#include "plots/_.c"
#include "genetics/_.c"
#include "neurals/_.c"
#include "blueprints/_.c"
#include "blocks/_.c"
#include "chunks/_.c"
#include "physics/_.c"
#include "voxes/_.c"
#include "vodes/_.c"
#include "lights/_.c"
#include "models/_.c"
#include "biomes/_.c"
#include "streaming/_.c"
#include "regions/_.c"
#include "tunks/_.c"
#include "particles/_.c"
#include "weathers/_.c"
#include "terrain/_.c"
#include "mountains/_.c"
#include "towns/_.c"
#include "vrays/_.c"
#include "characters/_.c"
#include "pets/_.c"
#include "dungeons/_.c"
#include "huds/_.c"
#include "space/_.c"
// user data
#include "users/_.c"
#include "combat/_.c"
#include "stats/_.c"
#include "skills/_.c"
#include "items/_.c"
#include "bodys/_.c"
#include "equipment/_.c"
#include "lores/_.c"
#include "achievements/_.c"
#include "dialogues/_.c"
// more user stuff
#include "actions/_.c"
#include "classes/_.c"
#include "jobs/_.c"
#include "races/_.c"
#include "clans/_.c"
#include "quests/_.c"
// gameplay
#include "pickups/_.c"
#include "crafting/_.c"
#include "farming/_.c"
#include "turrets/_.c"
#include "combat/_.c"
#include "maps/_.c"
#include "npcs/_.c"
// Placement
#include "terrain/npcs/_.c"
// on top
#include "controllers/_.c"
#include "imports/_.c"
#include "editor/_.c"
#include "debug/_.c"

// engine imports, besides sub modules, it's core is flecs
zox_begin_module(Zox) {
    zox_import_module(Core);
    zox_import_module(Events);
    zox_import_module(Transforms);
    zox_import_module(Networking);
    zox_import_module(Screens);
    zox_import_module(Inputs);
    zox_import_module(Apps);
    zox_import_module(Saves);
    zox_import_module(Graphics);
    zox_import_module(Assets);
    zox_import_module(Realms);
    zox_import_module(Games);
    zox_import_module(Players);
    zox_import_module(Triggers);
    zox_import_module(Nodes);
    zox_import_module(Slots);
    zox_import_module(Colorz);
    zox_import_module(Cameras);
    zox_import_module(Rendering);
    zox_import_module(Sounds);
    zox_import_module(Raycasts);
    zox_import_module(Lines);
    zox_import_module(Textures);
    zox_import_module(Cubes);
    zox_import_module(Musics);
    zox_import_module(Animations);
    zox_import_module(Bones);
    zox_import_module(Elements);
    zox_import_module(Lines2D);
    zox_import_module(Plots);
    zox_import_module(Genetics);
    zox_import_module(Neurals);
    zox_import_module(Blueprints);
    zox_import_module(Shapes);
    zox_import_module(Blocks);
    zox_import_module(Chunks);
    zox_import_module(Physics);
    zox_import_module(Voxes);
    zox_import_module(Vodes);
    zox_import_module(Lights);
    zox_import_module(Models);
    zox_import_module(Biomes);
    zox_import_module(Streaming);
    zox_import_module(Regions);
    zox_import_module(Tunks);
    zox_import_module(Particles);
    zox_import_module(Weathers);
    zox_import_module(Terrain);
    zox_import_module(Mountains);
    zox_import_module(Towns);
    zox_import_module(Vrays);
    zox_import_module(Characters);
    zox_import_module(Dungeons);
    zox_import_module(Huds);
    zox_import_module(Space);
    // user data
    zox_import_module(Users);
    zox_import_module(Combat);
    zox_import_module(Stats);
    zox_import_module(Skills);
    zox_import_module(Items);
    zox_import_module(Bodys);
    zox_import_module(Equips);
    zox_import_module(Actions);
    zox_import_module(Dialogues);
    zox_import_module(Quests);
    zox_import_module(Classes);
    zox_import_module(Jobs);
    zox_import_module(Races);
    zox_import_module(Clans);
    zox_import_module(Lores);
    zox_import_module(Achievements);
    // Placement
    zox_import_module(TerrainNpcs);
    // Gameplay
    zox_import_module(Pickups);
    zox_import_module(Crafting);
    zox_import_module(Farming);
    zox_import_module(Turrets);
    zox_import_module(Maps);
    zox_import_module(Npcs);
    zox_import_module(Pets);
    zox_import_module(Imports);
    zox_import_module(Editor);
    // space
#if defined(zoxm_players)
    zox_import_module(Controllers);
#endif
#if defined(zoxm_debug)
    zox_import_module(Debug);
#endif
} zox_end_module(Zox);

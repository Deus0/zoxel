#ifndef zoxm_zigels
#define zoxm_zigels

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "io/_.c"

zox_begin_module(Zigels)
    add_hook_files_load(load_files_fonts);
    add_hook_spawn_prefabs(spawn_prefabs_zigels);
    define_components_zigels(world);
    zox_filter(
        fonts,
        [none] FontTexture,
        [out] textures.GenerateTexture)
    zox_system_ctx(
        FontTextureSystem,
        zoxp_text,
        fonts,
        [in] textures.GenerateTexture,
        [in] ZigelIndex,
        [in] colorz.Color,
        [in] colorz.SecondaryColor,
        [in] rendering.TextureSize,
        [in] zigels.FontThickness,
        [in] zigels.FontOutlineThickness,
        [out] textures.TextureData,
        [out] rendering.TextureDirty,
        [none] FontTexture
    );
zox_end_module(Zigels)

#endif
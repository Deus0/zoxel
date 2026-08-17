/*
 * +------------------------------------------------------------------+
 * | Zox Module: Saves                                                |
 * |                                                                  |
 * |  Serialization - Profiles - Persistence - Slots - Save State     |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 * TODO:
 *
 *  - TransformDirty state for saving character transform
 *  - When Saving, open file when SaveDirty is active
 *      - SaveDirty -> Saving + Open File
 *      Then for each module, implement Saving + Tag
 *      For Loading, do same thing
 *          - except Loading is just triggered from the game systems
 *      For Example, when spawning Chunk is Initialized
 *          -just check if file exists
 *          - if it does, spawn and set Loading State and Filepath
 *          - Otherwise we set Generate flag
 *
 */
#ifndef zox_saves
#define zox_saves

#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Saves) {
    zox_define_components_saves(world);
    zox_define_systems_saves(world);
} zox_end_module(Saves);

#endif

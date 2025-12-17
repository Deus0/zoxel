#ifndef zoxm_ui
#define zoxm_ui

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "core/_.c"
#include "zigels/_.c"
#include "texts/_.c"
#include "elements2/_.c"
#include "elements3/_.c"
#include "windows/_.c"
#include "touch/_.c"

zox_begin_module(Elements)
    define_components_elements(world);
    zox_import_module(ElementsCore);
    zox_import_module(Zigels);
    zox_import_module(Texts);
    zox_import_module(Elements2);
    zox_import_module(Elements3);
    zox_import_module(Windows);
    zox_import_module(Touch);
zox_end_module(Elements)

#endif
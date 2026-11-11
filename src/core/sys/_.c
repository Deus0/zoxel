#include "deactivate.c"
#include "initialize.c"
#include "update.c"
#include "node.c"

void zox_systems_core(ecs* world) {
    zox_system(
        pre_initialize_system,
        zoxp_remove,
        [none] core.PreInitialize,
    );
    zox_system(
        initialize_system,
        zoxp_remove,
        [none] core.Initialize,
    );
    zox_system_1(
        initialize_event_system,
        zoxp_spawn,
        [in] core.InitializeEvent,
        [none] core.Initialize,
    );
    zox_system(
        dirty_system,
        zoxp_remove,
        [none] core.Dirty,
    );
    // !Dirty: both run deferred in zoxp_remove, so a Dirty on the frame after another queued
    //  [remove Dirty, add Update, remove Update], losing the second update
    //  (i.e. a tooltip cleared then set on back to back frames kept its old glyphs, hidden)
    zox_system(
        update_system,
        zoxp_remove,
        [none] core.Update,
        [none] !core.Dirty,
    );
    zox_system(
        trigger_begin_system,
        zoxp_remove,
        [none] core.TriggerBegin,
    );
    zox_system(
        begin_system,
        zoxp_remove,
        [none] core.Begin,
    );
    zox_system(
        trigger_end_system,
        zoxp_remove,
        [none] core.TriggerEnd,
    );
    zox_system(
        end_system,
        zoxp_remove,
        [none] core.End,
    );
    zox_system(
        trigger_exit_system,
        zoxp_remove,
        [none] core.TriggerExit,
    );
    zox_system(
        exit_system,
        zoxp_remove,
        [none] core.Exit,
    );
}

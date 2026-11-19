#include "wfc.c"

void zox_events_models(ecs* world) {
    // model nodes run as their process reaches them (Begin, doc/nodes.md)
    zox_on_add(
        model_wfc_node_event,
        [none] nodes.NodeRun,
        [none] models.ModelRun,
        [none] core.Begin,
    );
}

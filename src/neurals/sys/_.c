#include "input_neuron_system.c"
#include "feed_forward_system.c"
#include "neuron_feed_forward_system.c"
#include "connection_render_system.c"
#include "neuron_render_system.c"

void zox_systems_neurals(ecs* world) {
    // zox_system(neuron_input_system, zoxp_update, [none] InputNeuron) //  [out] Transfer,
    zox_system(
        link_feed_forward_system,
        zoxp_update,
        [in] ConnectionData,
        [out] Weight,
        [out] Signal,
        [out] Transfer,
        [none] Connection
    );
    zox_system(
        neuron_feed_forward_system,
        zoxp_update,
        [out] Signal,
        [none] Neuron,
        [none] !OutputNeuron
    );

#ifdef zox_render_brain
    zox_system_1(
        neuron_render_system,
        zoxp_spawn,
        [in] transforms2.Position2,
        [none] Neuron
    );
    zox_system_1(
        connection_render_system,
        zoxp_spawn,
        [in] ConnectionData,
        [in] Weight,
        [in] Signal,
        [in] Transfer,
        [none] Connection
    );
#endif
}

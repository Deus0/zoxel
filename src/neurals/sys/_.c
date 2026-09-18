#include "input_neuron_system.c"
#include "feed_forward_system.c"
#include "neuron_feed_forward_system.c"
#include "connection_render_system.c"
#include "neuron_render_system.c"

void zox_systems_neurals(ecs* world) {
    // zox_system(NeuronInputSystem, zoxp_update, [none] InputNeuron) //  [out] Transfer,
    zox_system(
        LinkFeedForwardSystem,
        zoxp_update,
        [in] ConnectionData,
        [out] Weight,
        [out] Signal,
        [out] Transfer,
        [none] Connection
    );
    zox_system(
        NeuronFeedForwardSystem,
        zoxp_update,
        [out] Signal,
        [none] Neuron,
        [none] !OutputNeuron
    );

#ifdef zox_render_brain
    zox_system_1(
        NeuronRenderSystem,
        zoxp_spawn,
        [in] transforms2.Position2,
        [none] Neuron
    );
    zox_system_1(
        ConnectionRenderSystem,
        zoxp_spawn,
        [in] ConnectionData,
        [in] Weight,
        [in] Signal,
        [in] Transfer,
        [none] Connection
    );
#endif
}

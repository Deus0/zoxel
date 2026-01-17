zox_sys2(NeuronFeedForwardSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Signal);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(Signal, signal);

        if (!signal->value) {
            continue; // only progress if signal is enabled
        }

        if (send_signals_from_neuron(world, e, signal->value)) {
#ifdef zox_log_neurals
            zox_log(" + s from n [%f]\n", signal->value)
#endif
            signal->value = 0;
        }
    }
} zox_sys_end(NeuronFeedForwardSystem);
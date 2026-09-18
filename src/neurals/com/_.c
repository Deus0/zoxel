zox_tag(Brain);
zox_tag(Neuron);
zox_tag(Connection);
zox_tag(InputNeuron);
zox_tag(OutputNeuron);
zoxc_float(Weight);                     //! The weight of the neural connection
zoxc_float(Signal);                     //! If the signal is sending
zoxc_float(Transfer);                   //! The signal value of a connection
zoxc_entity2(ConnectionData);     //! The weight of the neural connection
zoxc_byte(BrainDirty);
zoxc_entity(BrainLink);
zoxc_arrayd(BrainInputs, entity)
zoxc_arrayd(BrainOutputs, entity)
zoxc_arrayd(BrainLinks, entity)

void zox_components_neurals(ecs* world) {
    zoxd_tag(Brain);
    zoxd_tag(Neuron);
    zoxd_tag(Connection);
    zoxd_tag(InputNeuron);
    zoxd_tag(OutputNeuron);
    zoxd_float(Weight);
    zoxd_float(Signal);
    zoxd_float(Transfer);
    zoxd_entity2(ConnectionData);
    zoxd_byte(BrainDirty);
    zoxd_entity(BrainLink);
    zoxd_arrayd(BrainInputs);
    zoxd_arrayd(BrainOutputs);
    zoxd_arrayd(BrainLinks);
}

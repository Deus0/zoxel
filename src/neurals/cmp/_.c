zox_tag(Brain);
zox_tag(Neuron);
zox_tag(Connection);
zox_tag(InputNeuron);
zox_tag(OutputNeuron);
zoxc_float(Weight);                     //! The weight of the neural connection
zoxc_float(Signal);                     //! If the signal is sending
zoxc_float(Transfer);                   //! The signal value of a connection
zoxc(ConnectionData, entity2);     //! The weight of the neural connection
zoxc_byte(BrainDirty);
zoxc_entity(BrainLink);
zoxc_arrayd(BrainInputs, ecs_entity_t)
zoxc_arrayd(BrainOutputs, ecs_entity_t)
zoxc_arrayd(BrainLinks, ecs_entity_t)

void define_components_neurals(ecs* world) {
    zoxd_tag(Brain);
    zoxd_tag(Neuron);
    zoxd_tag(Connection);
    zoxd_tag(InputNeuron);
    zoxd_tag(OutputNeuron);
    zoxd_float(Weight);
    zoxd_float(Signal);
    zoxd_float(Transfer);
    zoxd(ConnectionData);
    zoxd_byte(BrainDirty);
    zoxd_entity(BrainLink);
    zoxd_arrayd(BrainInputs);
    zoxd_arrayd(BrainOutputs);
    zoxd_arrayd(BrainLinks);
}
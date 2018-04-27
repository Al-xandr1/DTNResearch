network RegularRealMobilityNetwork extends CommonRoutingNetwork
{
    submodules:
        host[numHosts]: RegularRealHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

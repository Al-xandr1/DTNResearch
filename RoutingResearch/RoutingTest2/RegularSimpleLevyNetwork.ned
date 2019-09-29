network RegularSimpleLevyNetwork extends CommonRoutingNetwork
{
    submodules:
        host[numHosts]: RegularSimpleLevyHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }    
}

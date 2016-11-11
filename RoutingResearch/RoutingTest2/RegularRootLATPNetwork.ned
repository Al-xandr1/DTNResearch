network RegularRootLATPNetwork extends CommonRoutingNetwork
{
    submodules:
        host[numHosts]: RegularRootLATPHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

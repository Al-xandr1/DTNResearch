network RegularSelfSimLATPNetwork  extends CommonRoutingNetwork
{
    submodules:
        host[numHosts]: RegularSelfSimLATPHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

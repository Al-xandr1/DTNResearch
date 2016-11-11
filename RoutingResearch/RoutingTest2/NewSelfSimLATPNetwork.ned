network NewSelfSimLATPNetwork  extends CommonRoutingNetwork
{
    submodules:
        host[numHosts]: NewSelfSimLATPHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

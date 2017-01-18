network NewRealMobilityNetwork extends CommonRoutingNetwork
{
    submodules:
        host[numHosts]: NewRealMobilityHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

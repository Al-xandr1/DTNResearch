network LevyHotSpotsLATPNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
    submodules:
        host[numHosts]: LevyHotSpotsLATPHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

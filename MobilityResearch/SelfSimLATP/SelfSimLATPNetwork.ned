network SelfSimLATPNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
    submodules:
        host[numHosts]: SelfSimLATPHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
}

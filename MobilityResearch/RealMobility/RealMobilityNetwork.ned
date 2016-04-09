network RealMobilityNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
    submodules:
        host[numHosts]: RealMobilityHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                indexOfNode = index;
        }
}

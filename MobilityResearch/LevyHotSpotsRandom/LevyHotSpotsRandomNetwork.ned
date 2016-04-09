network LevyHotSpotsRandomNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
    submodules:
        host[numHosts]: LevyHotSpotsRandomHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                indexOfNode = index;
        }
}

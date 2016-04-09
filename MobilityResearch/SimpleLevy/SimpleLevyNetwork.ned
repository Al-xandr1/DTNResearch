network SimpleLevyNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
    submodules:
        host[numHosts]: SimpleLevyHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                indexOfNode = index;
        }
}

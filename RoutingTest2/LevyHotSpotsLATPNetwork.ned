import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

network LevyHotSpotsLATPNetwork
{
    parameters:
        int numHosts;
        double interconnectionRadius = default(300); // radius of transmitters of each node in meters
        
        @display("bgb=600,400");
        
        @class(RoutingDaemon);
        
    submodules:
        host[numHosts]: MobileHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                indexOfNode = index;
        }
}

import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

network LevyHotSpotsLATPNetwork
{
    parameters:
        int numHosts;
        double interconnectionRadius = default(500); // radius of transmitters of each node in meters
        double timeOfCollection = default(86400); //time duration for collection of statistics (one day by default) 
        
        @display("bgb=600,400");
        
    submodules:
        routing: RoutingDaemon;
        
        host[numHosts]: MobileHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                indexOfNode = index;
        }
        
        collector: StatisticsCollector;
}

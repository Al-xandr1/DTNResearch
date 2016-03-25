import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

network RoutingLevyHotSpotsLATPNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
        double interconnectionRadius = default(700); 	// radius of transmitters of each node in meters
        
        // parameters for daily root change
        double dayDuration @unit(s) = default(43200s);  // by default 12 hours;
        int newHS_inRoot = default(1);
        
        //statistic collection
        int countOfDays = default(5); 					// number of days will be accounted for collection statistics of total time coonection per a day  
        
    submodules:
        host[numHosts]: RoutingMobileHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
        
        routing: RoutingDaemon;
        collector: StatisticsCollector;
}

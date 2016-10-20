network NewSelfSimLATPNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
        double interconnectionRadius = default(700); 	// radius of transmitters of each node in meters
        
        // heuristic's parameters (hysteresis)
        double trustTimeThresholdLow @unit(s) = default(0s);  // Low bound of trust time for LET algorithm (for hysteresis)
        double trustTimeThresholdHigh @unit(s) = default(0s); // High bound of trust time for LET algorithm (for hysteresis) 
        													  // Available values [0; +infinity]. Note: -1 means infinity
        // parameters for daily root change
        double dayDuration @unit(s) = default(43200s);  // by default 12 hours;
        
        // statistic collection
        int countOfDays = default(5); 					// number of days will be accounted for collection statistics of total time coonection per a day  
        
    submodules:
        host[numHosts]: NewSelfSimLATPHost {
            parameters:
                @display("p=300,300;r=,,#707070");
                NodeID_ = index;
        }
        
        routing: RoutingDaemon;
}

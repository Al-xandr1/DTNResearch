module RegularRootLATPHost
{
    parameters:
        string mobilityType = default("LevyHotSpotsRandom");
        
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(0);
        
		@class(MobileHost);
        
        // parameter for intencity of packet generation (Poisson flow)
        double timeslot = default(50);
    	double lambda = default(0.3);
    	
    	double rootPersistence = default(0.5); // Coefficient of persistence for locations. 
    							 		     // Available values [0, 1]
    
    gates:
        input in @directIn;
        
    submodules:
        mobility: RegularRootLATP {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_;
        }        
}

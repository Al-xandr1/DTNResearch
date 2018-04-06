module RegularSelfSimLATPHost
{
    parameters:
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(0);
        
		@class(MobileHost);
        
        // parameter for intencity of packet generation (Poisson flow)
        double timeslot = default(50);
    	double lambda = default(0.3);
    
    gates:
        input in @directIn;
        
    submodules:
        mobility: RegularSelfSimLATP {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_;
        }        
}

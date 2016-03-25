import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

//
// A host for demonstrating mobility models only -- it contains
// no protocol layers at all.
//
module RoutingMobileHost
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
    
    gates:
        input in @directIn;
        
    submodules:
        mobility: RegularRootLATP {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_;
        }        
}

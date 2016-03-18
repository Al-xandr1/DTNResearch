import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

//
// A host for demonstrating mobility models only -- it contains
// no protocol layers at all.
//
module MobileHost
{
    parameters:
        string mobilityType = default("LevyHotSpotsLATP");

		@class(MobileHost);
        @node();
        @display("i=device/pocketpc_s");
        
        int indexOfNode = default(0);
        
        // parameter for intencity of packet generation (Poisson flow)
        double timeslot = default(50);
    	double lambda = default(0.3);
    
    gates:
        input in @directIn;
            
    submodules:
        mobility: LevyHotSpotsLATP {
            parameters:
                @display("is=s;p=72,44");
                NodeID = indexOfNode;
        }
}

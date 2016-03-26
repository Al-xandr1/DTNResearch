import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

//
// A host for demonstrating mobility models only -- it contains
// no protocol layers at all.
//
module SelfSimMobileHost
{
    parameters:
        
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(0);
        
    submodules:
        mobility: SelfSimLATP {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_;
        }
}

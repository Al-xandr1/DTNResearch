module RealMobilityHost
{
    parameters:
        string mobilityType = default("RealMobility");
        
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(0);
        
    submodules:
        mobility: RealMobility {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_; 
        }
}

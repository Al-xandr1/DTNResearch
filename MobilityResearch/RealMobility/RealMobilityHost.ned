module RealMobilityHost
{
    parameters:
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(-1);
        
    submodules:
        mobility: RealMobility {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_; 
        }
}

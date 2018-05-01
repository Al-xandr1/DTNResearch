module LevyHotSpotsLATPHost
{
    parameters:
        string mobilityType = default("LevyHotSpotsLATP");
        
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(-1);
        
    submodules:
        mobility: LevyHotSpotsLATP {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_;
        }
}

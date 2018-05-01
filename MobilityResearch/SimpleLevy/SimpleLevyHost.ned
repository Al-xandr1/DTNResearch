module SimpleLevyHost
{
    parameters:
        string mobilityType = default("SimpleLevyMobility");
        
        @node();
        @display("i=device/pocketpc_s");
        
        int indexOfNode = default(-1);
        
    submodules:
        mobility: SimpleLevyMobility {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = indexOfNode;
        }
}

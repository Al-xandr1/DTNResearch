module LevyHotSpotsRandomHost
{
    parameters:
        string mobilityType = default("LevyHotSpotsRandom");
        
        @node();
        @display("i=device/pocketpc_s");
        
        int indexOfNode = default(0);
        
    submodules:
        mobility: LevyHotSpotsRandom {
            parameters: 
                @display("is=s;p=72,44");
            	fileSuffix = indexOfNode;
        }
}

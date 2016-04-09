module RealMobilityHost
{
    parameters:
        string mobilityType = default("RealMobility");
        
        @node();
        @display("i=device/pocketpc_s");
        
        int indexOfNode = default(0);
        
    submodules:
        mobility: RealMobility {
            parameters: 
            	fileSuffix = indexOfNode;
        }
}

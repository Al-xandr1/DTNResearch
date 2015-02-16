import inet.mobility.contract.IMobility;
import inet.mobility.single.BonnMotionMobility;

//
// A host for demonstrating mobility models only -- it contains
// no protocol layers at all.
//
module MobileHost
{
    parameters:
        string mobilityType = default("LevyMobility");
        
        @node();
        @display("i=device/pocketpc_s");
        @statistic[coordX](title="coord X"; source="xCoord(mobilityPos(mobilityStateChanged))"; record=vector; interpolationmode=linear);
        @statistic[coordY](title="coord Y"; source="yCoord(mobilityPos(mobilityStateChanged))"; record=vector; interpolationmode=linear);
        //@statistic[coordZ](title="coord Z";source="zCoord(mobilityPos(mobilityStateChanged))";record=vector; interpolationmode=linear);
        
        int indexOfNode = default(0);
    submodules:
        mobility: <mobilityType> like IMobility;
//        mobility: LevyMobility {
//            parameters: 
//            	fileSuffix = indexOfNode;
//        }
}

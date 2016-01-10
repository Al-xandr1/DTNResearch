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

        @node();
        @display("i=device/pocketpc_s");
        int indexOfNode = default(0);
    submodules:
        // mobility: <mobilityType> like IMobility;
        mobility: LevyHotSpotsLATP {
            parameters:
                @display("is=s;p=72,44");
                fileSuffix = indexOfNode;
        }
}

import inet.mobility.common.MovingMobilityBase;

simple RegularRootLATP extends LevyHotSpotsLATP
{
    parameters:
        @class(RegularRootLATP);
    
    gates:
        input in @directIn;        
}

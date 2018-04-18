import inet.mobility.common.MovingMobilityBase;

simple RegularRealMobility extends RealMobility
{
    parameters:
        @class(RegularRealMobility);
    
    gates:
        input in @directIn;        
}

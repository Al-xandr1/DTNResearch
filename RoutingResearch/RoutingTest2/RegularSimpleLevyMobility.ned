import inet.mobility.common.MovingMobilityBase;

simple RegularSimpleLevyMobility extends SimpleLevyMobility
{
    parameters:
        @class(RegularSimpleLevyMobility);
    
    gates:
        input in @directIn;        
}

import inet.mobility.common.MovingMobilityBase;

simple RegularSelfSimLATP extends SelfSimLATP
{
    parameters:
        @class(RegularSelfSimLATP);
    
    gates:
        input in @directIn;      
}

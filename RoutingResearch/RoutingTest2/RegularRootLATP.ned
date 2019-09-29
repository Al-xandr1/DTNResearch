import inet.mobility.common.MovingMobilityBase;

simple RegularRootLATP extends LevyHotSpotsLATP
{
    parameters:
        @class(RegularRootLATP);
            
        bool repetitionOfTraceEnabled = default(false); // enable daily trace repetition 
    
    gates:
        input in @directIn;      
}

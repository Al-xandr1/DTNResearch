import inet.mobility.common.MovingMobilityBase;

simple RegularSelfSimLATP extends SelfSimLATP
{
    parameters:
        @class(RegularSelfSimLATP);
        
        bool repetitionOfTraceEnabled = default(false); // enable daily trace repetition 
    
    gates:
        input in @directIn;      
}

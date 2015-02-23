import inet.mobility.common.MovingMobilityBase;

simple LevyMobility extends MovingMobilityBase
{
    parameters:
        //Ќачальное положение задаетс€ в методе MobilityBase::setInitialPosition()
        bool initFromDisplayString = default(true);
        volatile double speed @unit(mps) = default(mps); // use uniform(minSpeed, maxSpeed) or another distribution
        volatile double waitTime @unit(s) = default(0s); // wait time between reaching a target and choosing a new one
        @class(LevyMobility);
        
        bool useHotSpots = default(false);
        
        //statistic collection
    	string traceFileName = default("traceOfNode");
    	int fileSuffix = default(0);
}

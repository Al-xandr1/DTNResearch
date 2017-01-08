import inet.mobility.common.MovingMobilityBase;

simple RealMobility extends MovingMobilityBase
{
    parameters:
        //Ќачальное положение задаетс€ в методе MobilityBase::setInitialPosition()
        bool initFromDisplayString = default(false);
        //volatile double speed @unit(mps) = default(mps); 	// use uniform(minSpeed, maxSpeed) or another distribution (fake parameter - used by base classes)
        //volatile double waitTime @unit(s) = default(0s); 	// wait time between reaching a target and choosing a new one  (fake parameter - used by base classes)
        //constraintAreaMinX @unit(m) = default(-10000m); 	// min x position of the constraint area, unconstrained by default (negative infinity)
        //constraintAreaMinY @unit(m) = default(-10000m); 	// min y position of the constraint area, unconstrained by default (negative infinity)
        //constraintAreaMinZ @unit(m) = default(0m); 			// min z position of the constraint area, unconstrained by default (negative infinity)
        //constraintAreaMaxX @unit(m) = default(10000m); 		// max x position of the constraint area, unconstrained by default (positive infinity)
        //constraintAreaMaxY @unit(m) = default(10000m); 		// max y position of the constraint area, unconstrained by default (positive infinity)
        //constraintAreaMaxZ @unit(m) = default(0m); 			// max z position of the constraint area, unconstrained by default (positive infinity)
	
		@class(RealMobility);
 
        //statistic collection
    	string traceFileName = default("traceOfNode");
    	int NodeID = default(0);
}

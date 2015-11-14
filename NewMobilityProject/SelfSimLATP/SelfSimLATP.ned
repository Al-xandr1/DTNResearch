import inet.mobility.common.MovingMobilityBase;

simple SelfSimLATP extends MovingMobilityBase
{
    parameters:
        //Ќачальное положение задаетс€ в методе MobilityBase::setInitialPosition()
        bool initFromDisplayString = default(false);
        volatile double speed @unit(mps) = default(mps); // use uniform(minSpeed, maxSpeed) or another distribution (fake parameter - used by base classes)
        volatile double waitTime @unit(s) = default(0s); // wait time between reaching a target and choosing a new one  (fake parameter - used by base classes)
        
        constraintAreaMinX @unit(m) = default(-10000m); // min x position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMinY @unit(m) = default(-10000m); // min y position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMinZ @unit(m) = default(0m); // min z position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMaxX @unit(m) = default(10000m); // max x position of the constraint area, unconstrained by default (positive infinity)
        constraintAreaMaxY @unit(m) = default(10000m); // max y position of the constraint area, unconstrained by default (positive infinity)
        constraintAreaMaxZ @unit(m) = default(0m); // max z position of the constraint area, unconstrained by default (positive infinity)

        @class(SelfSimLATP);
 
        double ciP  = default(1);       // scale coeff (c) in Levy PAUSE disttibution - 1 in external research paper
    	double aliP = default(1);       // power magnitude (alpha) in Levy PAUSE distribution - 0.5 in external research paper
    	double aciP = default(0.001);	// accurancy for Levy PAUSE distribution 
 
 
        double powAforHS = default(2);		// is a fixed floating number constant within 0 to infinity - power for distance between HS
        double powAforWP = default(2);		// is a fixed floating number constant within 0 to infinity - power for distance between WP
        
        //statistic collection
        bool wayPointFormat = default(false); // fasle - points record as a trace: t x y
        									  // true  - points record as a waypoints: x y t t
    	string traceFileName = default("traceOfNode");
    	int fileSuffix = default(0);
}

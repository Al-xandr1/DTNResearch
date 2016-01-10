import inet.mobility.common.MovingMobilityBase;

simple LevyHotSpotsLATP extends MovingMobilityBase
{
    parameters:
        //Ќачальное положение задаетс€ в методе MobilityBase::setInitialPosition()
        bool initFromDisplayString = default(false);
        volatile double speed @unit(mps) = default(mps);  // use uniform(minSpeed, maxSpeed) or another distribution (fake parameter - used by base classes)
        volatile double waitTime @unit(s) = default(0s);  // wait time between reaching a target and choosing a new one  (fake parameter - used by base classes)
        constraintAreaMinX @unit(m) = default(-10000m);   // min x position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMinY @unit(m) = default(-10000m);   // min y position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMinZ @unit(m) = default(0m);        // min z position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMaxX @unit(m) = default(10000m);    // max x position of the constraint area, unconstrained by default (positive infinity)
        constraintAreaMaxY @unit(m) = default(10000m);    // max y position of the constraint area, unconstrained by default (positive infinity)
        constraintAreaMaxZ @unit(m) = default(0m);        // max z position of the constraint area, unconstrained by default (positive infinity)

        @class(LevyHotSpotsLATP);
 
 	    double ciJ  = default(1);       // scale coeff (c) in Levy LENGTH disttibution - 10 in external research paper
    	double aliJ = default(1);       // power magnitude (alpha) in Levy LENGTH distribution - 0.5, 1.0, 1.5 in external research paper
     	double aciJ = default(0.001);   // accurancy for Levy LENGTH distribution 
 
    	double ciP  = default(1);       // scale coeff (c) in Levy PAUSE disttibution - 1 in external research paper
    	double aliP = default(1);       // power magnitude (alpha) in Levy PAUSE distribution - 0.5 in external research paper
    	double aciP = default(0.001);	// accurancy for Levy PAUSE distribution 
    	
    	string specification = default("LevyHotSpotsLATP"); // concrete specification for Levy mobility 
    												  // available values = SimpleLevy | LevyHotSpotsRandom | LevyHotSpotsLATPCenterLogic | LevyHotSpotsLATP | LevyHotSpotsLATPPathCounts 
        double powA = default(1);		// is a fixed floating number constant within 0 to infinity - power for distance
        
        //statistic collection
    	string traceFileName = default("traceOfNode");
    	int fileSuffix = default(0);
}

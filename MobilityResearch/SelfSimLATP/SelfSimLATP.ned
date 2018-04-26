import inet.mobility.common.MovingMobilityBase;

simple SelfSimLATP extends MovingMobilityBase
{
    parameters:
        //Ќачальное положение задаетс€ в методе MobilityBase::setInitialPosition()
        bool initFromDisplayString = default(false);
        volatile double speed @unit(mps) = default(uniform(20mps,50mps)); 	// use uniform(minSpeed, maxSpeed) or another distribution (fake parameter - used by base classes)
        volatile double waitTime @unit(s) = default(uniform(3s,8s)); 		// wait time between reaching a target and choosing a new one  (fake parameter - used by base classes)
        constraintAreaMinX @unit(m) = default(-10000m); 					// min x position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMinY @unit(m) = default(-10000m); 					// min y position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMinZ @unit(m) = default(0m); 							// min z position of the constraint area, unconstrained by default (negative infinity)
        constraintAreaMaxX @unit(m) = default(10000m); 						// max x position of the constraint area, unconstrained by default (positive infinity)
        constraintAreaMaxY @unit(m) = default(10000m); 						// max y position of the constraint area, unconstrained by default (positive infinity)
        constraintAreaMaxZ @unit(m) = default(0m); 							// max z position of the constraint area, unconstrained by default (positive infinity)

        @class(SelfSimLATP);
 
        double ciP  = default(1);       // scale coeff (c) in Levy PAUSE disttibution - 1 in external research paper
    	double aliP = default(1);       // power magnitude (alpha) in Levy PAUSE distribution - 0.5 in external research paper
     	double deltaXP = default(1);    // size of grid 
 		double joinP = default(4);      // merge of grids' cells factor
 		
 		double kForSpeed = default(31.1457);    // scale parameter for speed equation. Default value setted for KAIST (is computed by cAlfaFitting utility) 
 		double roForSpeed = default(0.504349);  // power parameter for speed equation. Default value setted for KAIST (is computed by cAlfaFitting utility) 
 
        double powAforHS = default(2);		// is a fixed floating number constant within 0 to infinity - power for distance between HS
        double powAforWP = default(2);		// is a fixed floating number constant within 0 to infinity - power for distance between WP
        
    	int NodeID = default(0); 
}

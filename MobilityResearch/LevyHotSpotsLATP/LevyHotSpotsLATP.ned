import inet.mobility.common.MovingMobilityBase;

simple LevyHotSpotsLATP extends MovingMobilityBase
{
    parameters:
        @class(LevyHotSpotsLATP);
 
 	    double ciJ  = default(1);       // scale coeff (c) in Levy LENGTH disttibution - 10 in external research paper
    	double aliJ = default(1);       // power magnitude (alpha) in Levy LENGTH distribution - 0.5, 1.0, 1.5 in external research paper
     	double deltaXJ = default(1);    // size of grid 
 		double joinJ = default(4);      // merge of grids' cells factor

    	double ciP  = default(1);       // scale coeff (c) in Levy PAUSE disttibution - 1 in external research paper
    	double aliP = default(1);       // power magnitude (alpha) in Levy PAUSE distribution - 0.5 in external research paper
     	double deltaXP = default(1);    // size of grid 
 		double joinP = default(4);      // merge of grids' cells factor    	
 		
 		double kForSpeed = default(31.1457);    // scale parameter for speed equation. Default value setted for KAIST (is computed by cAlfaFitting utility) 
 		double roForSpeed = default(0.504349);  // power parameter for speed equation. Default value setted for KAIST (is computed by cAlfaFitting utility)
 		int countOfFirstSkippedLongFlight = default(1); // count of FIRST LONG flights that will be skipped in CURRENT hotspot 
 		
        double powA = default(1);		// is a fixed floating number constant within 0 to infinity - power for distance
        
    	int NodeID = default(0);
}

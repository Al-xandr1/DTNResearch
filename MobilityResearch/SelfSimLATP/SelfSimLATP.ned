import inet.mobility.common.MovingMobilityBase;

simple SelfSimLATP extends MovingMobilityBase
{
    parameters:
        @class(SelfSimLATP);
 
        double ciP  = default(1);       // scale coeff (c) in Levy PAUSE disttibution - 1 in external research paper
    	double aliP = default(1);       // power magnitude (alpha) in Levy PAUSE distribution - 0.5 in external research paper
     	double deltaXP = default(1);    // size of grid 
 		double joinP = default(4);      // merge of grids' cells factor
 		
 		double kForSpeed = default(31.1457);    // scale parameter for speed equation. Default value setted for KAIST (is computed by cAlfaFitting utility) 
 		double roForSpeed = default(0.504349);  // power parameter for speed equation. Default value setted for KAIST (is computed by cAlfaFitting utility) 
 
        double powAforHS = default(2);		// is a fixed floating number constant within 0 to infinity - power for distance between HS
        double powAforWP = default(2);		// is a fixed floating number constant within 0 to infinity - power for distance between WP
        
    	int NodeID = default(-1); 
}

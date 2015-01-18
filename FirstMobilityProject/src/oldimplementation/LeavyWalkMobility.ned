package oldimplementation;

import inet.mobility.common.MovingMobilityBase;

simple LeavyWalkMobility extends MovingMobilityBase
{
    parameters:
        @class(LeavyWalkMobility);
        
        double c_lenght = default(10);
        double c_pause = default(1);
    	double alpha = default(1);				// [0.5;1.5]
    	double betta = default(0.5);
    	
    	double tauL @unit(km) = default(1 km);	//for truncation
    	double tauP @unit(s) = default(1000 s);	//for truncation
    	
    	double k = default(1);					//for speed 
    	double ro = default(0);					//for speed
    	
    	//statistic collection
    	string traceFileName = default("traceOfNode");
    	int fileSuffix = default(0);
}

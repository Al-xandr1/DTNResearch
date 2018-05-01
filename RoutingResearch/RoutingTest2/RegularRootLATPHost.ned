module RegularRootLATPHost
{
    parameters:
        @node();
        @display("i=device/pocketpc_s");
        
        int NodeID_ = default(-1);
        
		@class(MobileHost);
        
        // parameter for intencity of packet generation (Poisson flow)
        double timeslot = default(50);
    	double lambda = default(0.3);
    							 		       
    	bool useWaypointCounter = default(false); // флаг, показывающий нужно использовать счётчик путевых точек на локацию или нет 					 		     
    
    gates:
        input in @directIn;
        
    submodules:
        mobility: RegularRootLATP {
            parameters: 
                @display("is=s;p=72,44");
            	NodeID = NodeID_;
        }        
}

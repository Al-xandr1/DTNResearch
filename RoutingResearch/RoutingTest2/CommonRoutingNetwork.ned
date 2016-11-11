network CommonRoutingNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
        double interconnectionRadius = default(700); 	// radius of transmitters of each node in meters
        
		// heuristic's parameters 
        double LET_Threshold @unit(s) = default(0s);  	// bound of trust time for LET algorithm 
        										      	// Available values [0; +infinity]. Note: -1 means infinity
        										
		// ��������, ���������� ����� ��������� ������������        										      
        // 0 - ��� ����������� (��� ���������) - LET_Threshold ��������
        // 1 - ������������ ������ LET - LET_Threshold ����������� (����� "�������������)
        // 2 - ����������� ������ MFV - LET_Threshold �� ����� ������, �� ������������
        int usedHeuristics = default(0);		
                
        // parameters for daily root change
        double dayDuration @unit(s) = default(43200s);  // by default 12 hours;
        
        // statistic collection
        int countOfDays = default(5); 					// number of days will be accounted for collection statistics of total time coonection per a day
        // ��������, ������� �������� ��� ��������� ������������� 
        // ��������� countOfDays ��� ����� ������� � ������ �������������,
        // � ������� ������� ���� ���������� �� ������������� �� ����������
        bool useCODForStat = default(false);  
        
    submodules:
        routing: RoutingDaemon;        
}
network CommonRoutingNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
        // parameters for daily root change
        double dayDuration @unit(s) = default(43200s);  // by default 12 hours;

		// ��������, ���������� ����� ��������� ������������        										      
        // 0 - ��� ����������� (��� ���������) - LET_Threshold ��������
        // 1 - ������������ ������ LET - LET_Threshold ����������� (����� "�������������)
        // 2 - ����������� ������ MFV - LET_Threshold �� ����� ������, �� ������������
        int usedHeuristics = default(0);		
 
 		// heuristic's parameters 
		// bound of trust time for LET algorithm. Available values [0; +infinity]. Note: -1 means infinity
        double LET_Threshold @unit(s) = default(0s);  	
        										 
        // radius of transmitters of each node in meters
        double interconnectionRadius = default(700); 	
        
        // ��������, ������������ ���������� �� �� ��������� ���������� � �������� ��������� ��� ��������� �����
        // ��� ���������� ����������� ��������������� (�� � RootsPersistenceAndStatistics)
        bool useRootStatistics = default(true); 
        
        // ��������, ������������ ����� �� ������������ ����� ���������� ���������, 
        // � ������, ���� ������� ���������� �� ������� ���������.
        // True - ���� ������� ���������� �� �����������, �� ����� �������� �����
        // False - ���� ������� ���������� �� �����������, �� ����� ������������� ������
        bool enabledCommonRootStatistics = default(false);
        
        // ��������, ������������ ����� �� ������������ ������ ��: 
        // - ������������ �������� ������� �� ���� ��������� ��������� �� ���������� ����� ������� ������������� -  TRUE
        // - ������������� �������� ������� �� ���� ��������� ��������� �� ���������� ����� ������� ������������� - FALSE 
        bool useFixedHomeLocation = default(true);
        
        // statistic collection
        // number of days will be accounted for collection statistics of total time coonection per a day
        int countOfDays = default(5); 	
        				
        // ��������, ������� �������� ��� ��������� ������������� 
        // ��������� countOfDays ��� ����� ������� � ������ �������������,
        // � ������� ������� ���� ���������� �� ������������� �� ����������
        bool useCountOfDaysForStat = default(false);  
        
    submodules:
        routing: RoutingDaemon;        
}
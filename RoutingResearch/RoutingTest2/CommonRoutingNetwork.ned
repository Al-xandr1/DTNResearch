network CommonRoutingNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
        double interconnectionRadius = default(700); 	// radius of transmitters of each node in meters
        
		// heuristic's parameters 
        double LET_Threshold @unit(s) = default(0s);  	// bound of trust time for LET algorithm 
        										      	// Available values [0; +infinity]. Note: -1 means infinity
        										
		// параметр, показыющий какие эвристики использовать        										      
        // 0 - нет ограничений (все эвристики) - LET_Threshold работает
        // 1 - использовать только LET - LET_Threshold отключается (равен "бесконечности)
        // 2 - использоват только MFV - LET_Threshold не имеет смысла, не используется
        int usedHeuristics = default(0);		
                
        // parameters for daily root change
        double dayDuration @unit(s) = default(43200s);  // by default 12 hours;
        
        // параметр, показывающий используем ли мы собранную статистику о реальных маршрутах для генерации новых
        // ИЛИ используем коэффициент ПЕРСИСТЕНТНОСТИ (всё в RootsPersistenceAndStatistics)
        bool useRootStatistics = default(true); 
        
        // statistic collection
        int countOfDays = default(5); 					// number of days will be accounted for collection statistics of total time coonection per a day
        // параметр, который включает или выключает использование 
        // параметра countOfDays как порог времени в начале моделирования,
        // в течение которго сбор статистики по маршрутизации не происходит
        bool useCODForStat = default(false);  
        
    submodules:
        routing: RoutingDaemon;        
}
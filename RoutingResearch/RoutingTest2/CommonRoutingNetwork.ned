network CommonRoutingNetwork
{
    parameters:
        int numHosts;
        @display("bgb=600,400");
        
        // parameters for daily root change
        double dayDuration @unit(s) = default(43200s);  // by default 12 hours;

		// параметр, показыющий какие эвристики использовать        										      
        // 0 - нет ограничений (все эвристики) - LET_Threshold работает
        // 1 - использовать только LET - LET_Threshold отключается (равен "бесконечности)
        // 2 - использоват только MFV - LET_Threshold не имеет смысла, не используется
        int usedHeuristics = default(0);		
 
 		// heuristic's parameters 
		// bound of trust time for LET algorithm. Available values [0; +infinity]. Note: -1 means infinity
        double LET_Threshold @unit(s) = default(0s);  	
        										 
        // radius of transmitters of each node in meters
        double interconnectionRadius = default(700); 	
        
        // параметр, показывающий используем ли мы собранную статистику о реальных маршрутах для генерации новых
        // ИЛИ используем коэффициент ПЕРСИСТЕНТНОСТИ (всё в RootsPersistenceAndStatistics)
        bool useRootStatistics = default(true); 
        
        // параметр, показывающий можно ли использовать общую статистику маршрутов, 
        // в случае, если частную статистику не удалось прочитать.
        // True - если частная статистика не прочиталась, то будет читаться общая
        // False - если частная статистика не прочиталась, то будет сгенерирована ошибка
        bool enabledCommonRootStatistics = default(false);
        
        // параметр, показывающий нужно ли использовать логику по: 
        // - ФИКСИРОВАНИЮ домашней лоакции во всех очередных маршрутах на протяжении всего времени моделирования -  TRUE
        // - ГЕНЕРИРОВАНИЯ домашней лоакции во всех очередных маршрутах на протяжении всего времени моделирования - FALSE 
        bool useFixedHomeLocation = default(true);
        
        // statistic collection
        // number of days will be accounted for collection statistics of total time coonection per a day
        int countOfDays = default(5); 	
        				
        // параметр, который включает или выключает использование 
        // параметра countOfDays как порог времени в начале моделирования,
        // в течение которго сбор статистики по маршрутизации не происходит
        bool useCountOfDaysForStat = default(false);  
        
    submodules:
        routing: RoutingDaemon;        
}
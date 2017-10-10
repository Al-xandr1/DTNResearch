
simple RootsPersistenceAndStatistics
{
    parameters:
        // Данная логика вынесена в простой модуль только из-за возможности объявлять xml документ
        @class(RootsPersistenceAndStatistics);
        
      	// Coefficient of persistence for locations & other roots statistics
    	xml rootsPersistenceAndStatistics = default(xmldoc("Traces/roots_persistence_statistics.pst"));
}
network StatisticsCollectorNetwork
{
    submodules:
        collector:  StatisticsCollector2;
        calculator: StatisticsCalculator;
}

simple StatisticsCollector2
{
    parameters:
    	string packetsHistoryDoc = default("outTrace/packetsHistory.xml");
    	string ictHistoryDoc 	 = default("outTrace/ictHistory.xml");
    	string routeHistoryDoc 	 = default("outTrace/routeHistory.xml");
}

simple StatisticsCalculator
{
    parameters:
        string routeHistoryOneFileName   =   "outTrace/routeHistory.xml";
    	xml routeHistoryOne = default(xmldoc("outTrace/routeHistory.xml"));
        
        string routeHistoryTwoFileName   =   "outTrace/routeHistory_another.xml";
    	xml routeHistoryTwo = default(xmldoc("outTrace/routeHistory_another.xml"));
}

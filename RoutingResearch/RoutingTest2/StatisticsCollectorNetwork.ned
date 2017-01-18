network StatisticsCollectorNetwork
{
    submodules:
        collector:  StatisticsCollector2;
        calculator: StatisticsCalculator;
}

simple StatisticsCollector2
{
    parameters:
    	xml packetsHistoryDoc = default(xmldoc("outTrace/packetsHistory.xml"));
    	xml ictHistoryDoc = default(xmldoc("outTrace/ictHistory.xml"));
    	xml routeHistoryDoc = default(xmldoc("outTrace/routeHistory.xml"));
}

simple StatisticsCalculator
{
    parameters:
        string routeHistoryOneFileName   =   "outTrace/routeHistory.xml";
    	xml routeHistoryOne = default(xmldoc("outTrace/routeHistory.xml"));
        
        string routeHistoryTwoFileName   =   "outTrace/routeHistory_another.xml";
    	xml routeHistoryTwo = default(xmldoc("outTrace/routeHistory_another.xml"));
}

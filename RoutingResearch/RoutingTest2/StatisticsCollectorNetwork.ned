network StatisticsCollectorNetwork
{
    submodules:
        collector:  StatisticsCollector2;
//        calculator: StatisticsCalculator;
}

simple StatisticsCollector2
{
    //имена файлов беруться из DevelopmentHelper.h
}

simple StatisticsCalculator
{
    parameters:
        string routeHistoryOneFileName   =   "outTrace/routeHistory.xml";
    	xml routeHistoryOne = default(xmldoc("outTrace/routeHistory.xml"));
        
        string routeHistoryTwoFileName   =   "outTrace/routeHistory_another.xml";
    	xml routeHistoryTwo = default(xmldoc("outTrace/routeHistory_another.xml"));
}

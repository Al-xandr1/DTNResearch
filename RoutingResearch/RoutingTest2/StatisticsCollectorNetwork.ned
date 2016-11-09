network StatisticsCollectorNetwork
{
    submodules:
        collector: StatisticsCollector2;
}

simple StatisticsCollector2
{
    parameters:
    	xml packetsHistoryDoc = default(xmldoc("outTrace/packetsHistory.xml"));
    	xml ictHistoryDoc = default(xmldoc("outTrace/ictHistory.xml"));
    	xml routeHistoryDoc = default(xmldoc("outTrace/routeHistory.xml"));
}

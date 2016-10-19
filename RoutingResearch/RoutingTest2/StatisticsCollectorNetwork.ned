network StatisticsCollectorNetwork
{
    submodules:
        collector: StatisticsCollector2;
}

//todo remove
simple StatisticsCollector
{
    gates:
        input in @directIn;      
}

simple StatisticsCollector2
{
}

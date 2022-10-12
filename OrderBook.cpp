#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>


/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    cursor = orders.begin();

}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts() const
{
    return products;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;

    /* Search for current time */
    if ( timestamp.empty() )
    {
        timestamp = cursor -> timestamp;

        for ( auto it = cursor; it != orders.end(); ++it )
        {
            if ( it -> timestamp != cursor -> timestamp )
                break;

            if ( it -> orderType == type && it -> product == product )
            {
                orders_sub.push_back(*it);
            }
        }
    }
    else
    /* Search from some time */
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                              std::string product, 
                                              int timesteps_back)
{
    std::vector<OrderBookEntry> orders_sub;

    auto start  = cursor;
    auto fin    = cursor;
    auto timestamp = cursor -> timestamp;

    /** move back to find start */
    while ( timesteps_back > 0 )
    {
        if ( start == orders.begin() )
            break;

        if ( timestamp != start -> timestamp ) 
        {
            timesteps_back--;
            timestamp = start -> timestamp;

            if ( timesteps_back < 0 )
            {
                start++;
                break;
            }
        }
        --start;
    }
    /** move to the last record with the same timestamp like current one */
    for ( ; fin != orders.end(); ++fin )
    {
        if ( fin -> timestamp != cursor -> timestamp )
        {
            break;
        }
    }

    for ( auto it = start; it != fin; ++it )
    {
        if ( it -> orderType == type && it -> product == product )
        {
            orders_sub.push_back(*it);
        }
    }

    return orders_sub;
}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

std::string OrderBook::getCurrentTime() const
{
    if ( cursor != orders.end() )
        return cursor -> timestamp;

    return orders.rbegin()->timestamp;
}

void OrderBook::nextTime()
{
    auto it = cursor;

    for ( ; it != orders.end(); ++it )
    {
        if ( it -> timestamp != cursor -> timestamp )
            break;
    }

    cursor = it;
}


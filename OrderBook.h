#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts() const;
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp="");

    /** return vector of Orders according to the sent filters and back to some time steps from current cursor */
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              int timesteps_back);
        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);

        /** returns the timestamp of the timeframe where cursor is */
        std::string getCurrentTime() const;

        /** move cursor to the next timeframe */
        void nextTime();

    private:
        /** order data records */
        std::vector<OrderBookEntry> orders;         

        /** current time cursor position */
        std::vector<OrderBookEntry>::iterator cursor;

        /** all products in orders book */
        std::vector<std::string> products;


};

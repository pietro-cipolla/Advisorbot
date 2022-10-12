#pragma once

#include <vector>
#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"


class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private: 
        /** read and parse user command */ 
        bool getUserCommand (void);

        /** process help command */
        void processHelp ( const std::vector<std::string> &args );
        /** process prod command*/
        void processProd ( const std::vector<std::string> &args );
        /** process min command*/
        void processMin  ( const std::vector<std::string> &args );
        /** process max command*/
        void processMax  ( const std::vector<std::string> &args );
        /** process min/max command*/
        void processMinMax ( const std::string &prod, const std::string &bidask, const std::string &minmax );
        /** process avg command*/
        void processAvg  ( const std::vector<std::string> &args );
        /** process median command*/
        void processMed  ( const std::vector<std::string> &args );
        /** process predict command*/
        void processPred ( const std::vector<std::string> &args );
        /** process time command*/
        void processTime ( const std::vector<std::string> &args );
        /** process step command*/
        void processStep ( const std::vector<std::string> &args );

//        OrderBook orderBook{"20200317.csv"};
	OrderBook orderBook{"20200601.csv"};

};

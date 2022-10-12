#include <algorithm>
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "StringUtils.h"
#include "MerkelMain.h"


MerkelMain::MerkelMain()
{

}

/** helper function to print a message */
static void printMessage(const std::string &msg)
{
    std::cout << "advisorbot> " << msg << std::endl;
}

/** Function with main loop */
void MerkelMain::init()
{
    printMessage("Please enter a command, or help for a list of commands");

    /** Main loop */
    while(std::cin)
    {
        if ( !getUserCommand() )
            break;
    }
}

/** helper function to check number of arguments */
static bool check_args ( const std::vector<std::string> &args, size_t n )
{
    if ( args.size() != n )
    {
        printMessage ("Bad number of args! See help.");
        return false;
    }
    return true;
}

/** helper function to check the product is in database */
static bool check_product ( const OrderBook &orderBook, const std::string &prod )
{
    auto products = orderBook.getKnownProducts();

    if ( std::find(products.begin(),products.end(), prod) == products.end() )
    {
        printMessage ("No such product: " + prod);
        return false;
    }

    return true;
}

/** helper function to check input for `min` or `max` */
static bool check_minmax ( const std::string &minmax )
{
    if ( minmax != "min" && minmax != "max" )
    {
        printMessage ("Bad argument: " + minmax + ". Must be `min` or `max`.");
        return false;
    }

    return true;
}

/** helper function to check input for `bid` or `ask` */
static bool check_bidask ( const std::string &bidask )
{
    if ( bidask != "bid" && bidask != "ask" )
    {
        printMessage ("Bad argument: " + bidask + ". Must be `bid` or `ask`.");
        return false;
    }

    return true;
}

/** helper function to check input for integer */
static bool check_timestep ( const std::string &stimestep, int &itimestep )
{
    try 
    {
        itimestep = std::stoi(stimestep);

        if ( itimestep <= 0)
            throw std::exception();
    }
    catch (...)
    {
        printMessage ("Bad args! timesteps is wrong!");
        return false;
    }

    return true;
}


/** Process user command until `quit` */
bool MerkelMain::getUserCommand ( void )
{
    bool ret = true;

    std::vector<std::string> tokens;

    std::string line;

    std::cout << "user> ";

    std::getline(std::cin, line);

    line = trim(line);

    tokens = tokenise (line, ' ');

    if ( tokens.size() )
    {
        if ( tokens[0] == "help" )
            processHelp ( tokens );
        else if ( tokens[0] == "prod" )
            processProd ( tokens );
        else if ( tokens[0] == "min" )
            processMin ( tokens );
        else if ( tokens[0] == "max" )
            processMax ( tokens );
        else if ( tokens[0] == "avg" )
            processAvg ( tokens );
        else if ( tokens[0] == "predict" )
            processPred( tokens );
        else if ( tokens[0] == "time" )
            processTime( tokens );
        else if ( tokens[0] == "step" )
            processStep( tokens );
        else if ( tokens[0] == "median" )
            processMed( tokens );
        else if ( tokens[0] == "quit" )
            ret = false;
        else
            printMessage ("Error! Unknown command: "+tokens[0]);
    }

#if 0
    std::cout << "<<" << line   << ">>" << ", tokens: [";
    for ( auto it = tokens.begin();  it != tokens.end(); ++it )
        std::cout << *it << ((it+1) != tokens.end()?",":""); 
    std::cout << "]" << std::endl;
#endif

    return ret;
}

/** process help command */
void MerkelMain::processHelp ( const std::vector<std::string> &args )
{
    if ( args.size() == 1 )
    {
        printMessage ("The available commands are help,help <prod|min|max|avg|predict|time|step|median|quit>");
    }
    else
    if ( args.size() == 2 )
    {
        if ( args[1] == "prod" )
            printMessage("prod -> list of availabel products");
        else if ( args[1] == "min" )
            printMessage("min product bid/ask -> find minimum bid or ask for product in current time step");
        else if ( args[1] == "max" )
            printMessage("max product bid/ask -> find maximum bid or ask for product in current time step");
        else if ( args[1] == "avg" )
            printMessage("avg product bid/ask timesteps -> compute average ask or bid for the product over the time steps");
        else if ( args[1] == "predict" )
            printMessage("predict min/max product bid/ask -> predict min or max ask or bid for the product for the next time step");
        else if ( args[1] == "time" )
            printMessage("time -> state current time in dataset");
        else if ( args[1] == "step" )
            printMessage("step -> move to the next time step");
        else if ( args[1] == "median" )
            printMessage("median product bid/ask timesteps -> compute median ask or bid for the product over the time steps");
        else if ( args[1] == "quit" )
            printMessage("exit from the program");
        else
            printMessage ("No such command `" + args[1] + "`!");
    }
    else
    {
        printMessage ("Bad args for help command! You should use `help` or `help <cmd>` notation.");
    }
}

/** process prod command */
void MerkelMain::processProd ( const std::vector<std::string> &args )
{
    std::string answer; 

    auto products = orderBook.getKnownProducts();

    for ( auto it = products.begin(); it != products.end(); ++it )
    {
        answer += *it; 
        if ( (it+1) != products.end() )
            answer += ",";
    }

    if ( answer.length() )
        printMessage(answer);
    else
        printMessage("There are no available products!");
}

/** process min command */
void MerkelMain::processMin ( const std::vector<std::string> &args )
{
    if ( !check_args ( args, 3 ) )
        return ;

    processMinMax ( args[1], args[2], "min" );
}

/** process max command */
void MerkelMain::processMax ( const std::vector<std::string> &args )
{
    if ( !check_args ( args, 3 ) )
        return ;

    processMinMax ( args[1], args[2], "max" );
}

/** process min/max */
void MerkelMain::processMinMax ( const std::string &prod, const std::string &bidask, const std::string &minmax )
{
    if ( !check_product ( orderBook, prod ) )
        return;

    if ( !check_bidask  ( bidask ) ) 
        return ;

    auto orders = orderBook.getOrders ( bidask == "bid" ? OrderBookType::bid : OrderBookType::ask, prod); 

    if ( orders.size() == 0 )
    {
        printMessage ("No `" + bidask + "` at the " + orderBook.getCurrentTime() );

        return ;
    }
    auto mme = std::minmax_element ( orders.begin(), orders.end(), OrderBookEntry::compareByPriceAsc );

    printMessage ("The min " + bidask + " for " + prod + " is " + std::to_string(minmax=="min"?mme.first->price:mme.second->price));
}

/** process avg command */
void MerkelMain::processAvg ( const std::vector<std::string> &args )
{
    int timesteps; 

    if ( !check_args    ( args, 4 ) )
        return ;

    if ( !check_product ( orderBook, args[1] ) )
        return;

    if ( !check_bidask  ( args[2] ) )
        return;

    if ( !check_timestep ( args[3], timesteps ) )
        return ;

    double avg = 0;

    std::vector<OrderBookEntry> orders;

    if ( args[2] == "bid" )
        orders = orderBook.getOrders (  OrderBookType::bid, args[1], timesteps); 
    else
        orders = orderBook.getOrders (  OrderBookType::ask, args[1], timesteps); 

    //std::cout << orders.size() << " " << orders[0].timestamp << " " << orders[0].price << " " << orders.rbegin()->timestamp << " " << orders.rbegin()->price << std::endl;

    for ( auto order: orders )
        avg += order.price;
    if ( orders.size() )
        avg /= orders.size();

    printMessage("The average "+args[1] + " "+args[2]+" price over the last "+args[3]+" timesteps was "+std::to_string(avg));
}

/** process predict command - To predict a min/max price value we use moving average for 10 last time frames */
void MerkelMain::processPred ( const std::vector<std::string> &args )
{
    if ( !check_args    ( args, 4 ) )
        return ;

    if ( !check_minmax ( args[1] ) )
        return;

    if ( !check_product ( orderBook, args[2] ) )
        return;

    if ( !check_bidask  ( args[3] ) )
        return;


    std::vector<OrderBookEntry> orders;

    /** select 10 last time frames */
    if ( args[3] == "bid" )
        orders = orderBook.getOrders (  OrderBookType::bid, args[2], 10 ); 
    else
        orders = orderBook.getOrders (  OrderBookType::ask, args[2], 10 ); 

    auto timestamp = orders[0].timestamp;
    double pred_min = 0, pred_max = 0, min = orders[0].price, max = orders[0].price;
    int count = 0;

    /** calculate moving average */
    for ( auto order : orders )
    {
        if ( min > order.price )
            min = order.price;
        if ( max < order.price )
            max = order.price;

        if ( order.timestamp != timestamp )
        {
            //std::cout << timestamp << " min: " << min << std::endl;

            timestamp = order.timestamp;

            pred_min += min;
            pred_max += max;


            min = max = order.price;

            count ++;
        }
    }

    /** Process last time frame */
    count ++;

    //std::cout << orders.rbegin() -> timestamp << " min: " << min << " count=" << count << std::endl;

    pred_min += min;
    pred_max += max;

    pred_min /= count;
    pred_max /= count;

    printMessage("The predicted "+args[1] + " "+ args[2]+" "+args[3]+" price is "+std::to_string(args[1]=="min"?pred_min:pred_max));
}

/** process time command - print a current time step */
void MerkelMain::processTime ( const std::vector<std::string> &args )
{
    printMessage(orderBook.getCurrentTime());
}

/** process step command - move to the next time step */
void MerkelMain::processStep ( const std::vector<std::string> &args )
{
    orderBook.nextTime();

    printMessage("now at " + orderBook.getCurrentTime());
}

/** process median command - Calculate median price for last timesteps */
void MerkelMain::processMed ( const std::vector<std::string> &args )
{
    int timesteps; 

    if ( !check_args    ( args, 4 ) )
        return ;

    if ( !check_product ( orderBook, args[1] ) )
        return;

    if ( !check_bidask  ( args[2] ) )
        return;

    if ( !check_timestep ( args[3], timesteps ) )
        return ;

    double median = 0;

    std::vector<OrderBookEntry> orders;

    if ( args[2] == "bid" )
        orders = orderBook.getOrders (  OrderBookType::bid, args[1], timesteps); 
    else
        orders = orderBook.getOrders (  OrderBookType::ask, args[1], timesteps); 

    std::sort( orders.begin(), orders.end(), OrderBookEntry::compareByPriceAsc );

    median = orders[orders.size()/2].price;

    printMessage("The median "+args[1] + " "+args[2]+" price over the last "+args[3]+" timesteps was "+std::to_string(median));
}





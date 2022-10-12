CXX = g++ -std=c++11
CXXFLAGS = -g -Wall
OBJS = CSVReader.o MerkelMain.o OrderBook.o OrderBookEntry.o StringUtils.o main.o 
TARGET = advisorbot


all: $(TARGET)

$(TARGET): $(OBJS)
		$(CXX) -o $(TARGET) $(OBJS)

clean:;		$(RM) $(TARGET) core *.o

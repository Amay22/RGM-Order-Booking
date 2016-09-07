// RGM Order Book Pricer Problem
// Compile : g++ pricer.cpp
// Run : ./a.out [targer-size] [file-path]
//
// How did you choose your implementation language?
// Fast pithy and efficient code calls for C++. Financial Software should always be done in C++.
// I have used pass-by-reference and public libraries only so code is fast.
//
// What is the time complexity for processing an Add Order message?
// Add Order Message is effectively O(1) complexity as I have used a map which is ordered.
//
// What is the time complexity for processing a Reduce Order message?
// Reduce Order message complexity is O(1) as well because of the map.
//
// Although everytime a order is added or reduced transactions are performed which can
// go upto a complexity of O(n). Eventhough the orderbook is arranged in increasing prices.
//
// If your implementation were put into production and found to be too slow,
// what ideas would you try out to improve its performance?
// (Other than reimplementing it in a different language such as C or C++.)
//
// I would first right test cases as I have not done in this one. Will deal with slow after I test it.
// Avoid Bottlenecks.
// Better Garbage Collection for the 3 maps used namely: orderBook, asks, bids.
// Some values could be cached instead of being retrieved from map. Like the hghest ones or so.

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <queue>
#include <unordered_map>
// Not Recommended but I like to keep the code below clean
using namespace std;

// Comparator for bids (from high to low)
struct cmpBid { bool operator()(const double &a, const double &b) const { return a > b; } };

class Pricer{

		unsigned int targetSize;
		string timestamp;
		unsigned int bid;
		unsigned int ask;
		double income;
		double expense;
		unordered_map< string, pair<bool, pair<double, unsigned int> > > orderBook;
		map<double, unsigned int ,cmpBid> bids;
		map<double, unsigned int> asks;

		double getTotal(bool isAsk){
			double result = 0;
			unsigned int tmpTarget = targetSize;
			for(map<double, unsigned int>::iterator it= (isAsk ? asks.begin() : bids.begin()); tmpTarget > 0 ; it++){
				if(tmpTarget >= it->second){
					result += it->first * it->second;
					tmpTarget -= it->second;
				}else{
					result += it->first * tmpTarget;
					tmpTarget -= tmpTarget;
				}
			}
			return result;
		}

		void addOrder(const string orderID, const bool isAsk, const double price, const unsigned int count){
			orderBook[orderID] = make_pair(isAsk, make_pair(price, count));
			double curPrice = isAsk ? expense : income;
			if(isAsk){
				asks[price] += count;
				ask += count;
			}else{
				bids[price] += count;
				bid += count;
			}
			if(checkNumOfShares(isAsk)){
				double total = getTotal(isAsk);
				if(curPrice != total){
					printResult(timestamp, isAsk, total);
					if(isAsk){ expense = total; }
					else{ income = total; }
				}
			}
		}

		void reduceOrder(const string orderID, const unsigned int count){
			double reducePrice = orderBook[orderID].second.first;
			bool isAsk = orderBook[orderID].first;
			bool sufficientShares;
			double curPrice = isAsk ? expense : income;
			if(isAsk){
				sufficientShares =  ask < targetSize;
				asks[reducePrice] -= count;
				ask-=count;
				if(asks[reducePrice]==0){ asks.erase(reducePrice); }
			}else{
				sufficientShares = bid < targetSize;
				bids[reducePrice] -= count;
				bid-=count;
				if(bids[reducePrice]==0){ bids.erase(reducePrice); }
			}
			if((orderBook[orderID].second.second -= count) == 0){ orderBook.erase(orderID); }
			if(checkNumOfShares(isAsk)){
				double total = getTotal(isAsk);
				if(curPrice != total){
					printResult(timestamp, isAsk, total);
					if(isAsk){ expense = total; }
					else{ income = total; }
				}
			}else{
				if(!sufficientShares){
					if(isAsk){ expense = 0; }
					else{ income = 0; }
					printResult(timestamp, ask, 0);
				}
			}
		}

		bool checkNumOfShares(bool isAsk){ return isAsk ? ask >= targetSize : bid >= targetSize; }

		void printResult(const string timestamp, const bool isAsk, double price){
			cout << timestamp << " " << ((isAsk) ? 'B' : 'S') << " ";
    	price == 0 ? printf("NA\n") : printf("%.2f\n", price);
		}

		bool checkPositiveNum(const string num){
			for(int i = 0; i < num.length() ; i++){
				if(num.at(i) == '.'){ continue; }
				if(!isdigit(num.at(i))){ return false; }
			}
			return true;
		}

	public:

		Pricer(const unsigned int _targetSize){ targetSize = _targetSize; expense = 0; income = 0; bid = 0; ask = 0; }

		void processFeed(queue<string> ticks){
			while(!ticks.empty()){
				string line = ticks.front();
				ticks.pop();
				vector<string> tokens;
				istringstream iss(line);
			  string tok;
			  while(getline(iss, tok, ' ')) { tokens.push_back(tok); }
				timestamp = tokens[0];
				if(tokens[1].at(0) == 'A'){
					if(tokens.size() != 6){
						cerr<<"[" << line << "]:"<<" Should have 6 entries"<<endl;
					}else if(tokens[3].at(0) != 'S' && tokens[3].at(0) != 'B'){
						cerr<<"[" << line << "]:"<<" Should be buy or Sell"<<endl;
					}else if(!checkPositiveNum(tokens[4]) && !checkPositiveNum(tokens[5])){
						cerr<<"[" << line << "]:"<<" Should have positive values"<<endl;
					}else{
						addOrder(tokens[2], (tokens[3].at(0) == 'S'), stod(tokens[4]), stoul(tokens[5]));
					}
				}else if(tokens[1].at(0) == 'R'){
					if(tokens.size() != 4){
						cerr<<"[" << line << "]:"<<" Should have 4 entries"<<endl;
					}else if(!checkPositiveNum(tokens[3])){
						cerr<<"[" << line << "]:"<<" Should have positive values"<<endl;
					}else{
						reduceOrder(tokens[2],  stoul(tokens[3]));
					}
				}else{
					cerr<<"[" << line << "]:"<<" Error"<<endl;
				}
			}
		}
};


int main(int argc, char *argv[]){
	queue<string> ticks;
	if(argc < 2){
		cout<<"Please input target size. e.g. ./a.out [targer-size] [file-path]"<<endl;
		return 0;
	}else if(argc == 2){
		for(string line; getline(cin, line);){
			if(line.length() == 0){break;}
			ticks.push(line);
		}
	}else{
		ifstream targetFile;
		string line;
		targetFile.open(argv[2], ios::binary|ios::out);
		if(targetFile.is_open()){ while(getline(targetFile,line)){ ticks.push(line); } }
	}
	Pricer* pricer = new Pricer(stoi(argv[1]));
	pricer->processFeed(ticks);
	return 0;
}

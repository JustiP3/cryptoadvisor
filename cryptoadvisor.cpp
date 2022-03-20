#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <cstdlib>

/*
 * Program Goals:
 * Read text file with config and current holdings - done 
 * Fetch current crypto prices from api - bash script calls api, writes to text file then this program reads from text file - close enough
 * Calculate current values and compare to config - done 
 * Recommend trades to rebalance per config - done
 * Update config file with new balances if recommended trades were made - done
 * 
 * 
 * Refactor and technical goals:
 * Generalize the solution 
 * Implement linked list to allow variable numbers of coins defined in config.txt
 * Coin should be a class not a struct with static and instance methods 
 * 
 * 
 * ToDo / left off:
 * add actual percent values to the line that prints "you are holding..."
 * clean up main() - move to small helper functions 
 * 
 * 
*/ 

struct Coin 
{
	std::string name;
	std::string quant; 
	std::string price; 
	double usdval;
	std::string tbal;
};



void readconfig(std::string (&names)[10], std::string (&holdings)[10], std::string (&tbal)[10]) {
	std::ifstream input;
	input.open("config.txt");
	
	int linecount = 0; // index for the line number 

	for (std::string tp; getline(input, tp); ) {
			
		char p[tp.length()];		
		std::string temp = "";	
		unsigned int entries_index = 0;  // index of which element on the line 		
			

		for (int i = 0; i < sizeof(p); i++) {
			p[i] = tp[i]; // convert string to char array 

			if (p[i] == 32) {
				// if we hit the space
				//increment entries index and copy temp string to correct array based on line number					
				if (temp.length() > 0) {
					switch (linecount) {
					case 0:
					names[entries_index] = temp;
					break;
					case 1:
					holdings[entries_index] = temp;
					break;
					case 2:
					tbal[entries_index] = temp; 
					break;
					default:
					std::cout << "error in read config" << std::endl;
					break;		
				};							
				temp = "";
				entries_index++;	
				}	
			} else {
				// as long as this char is not space, copy into the temp string 
				temp.push_back(p[i]); // copy char into the temp string 
			}	
		}
		// if config line does not end in a space the final entry of the line needs to be copied
		if (temp.length() > 0) {
			switch (linecount) {
			case 0:
			names[entries_index] = temp;
			break;
			case 1:
			holdings[entries_index] = temp;
			break;
			case 2:
			tbal[entries_index] = temp; 
			break;
			default:
			std::cout << "error in read config" << std::endl;
			break;		
			};	
		}					
		linecount++;
	}

	input.close(); 	
}

std::string getsym(std::string line) {
	char p[line.length()];
	
    unsigned int i;
    for (i = 0; i < sizeof(p); i++) {
        p[i] = line[i];
    }
    
    int len; // will be 3,4, or 5
    if (p[13] == '"') {
		len = 3; 
	} else if (p[14] == '"') {
		len = 4; 
	} else if (p[15] == '"') {
		len = 5 ;
	} else {
		std::cout << "error" << std::endl; 
	}
	std::string ans;
	for (int i = 0; i < len; i++) {		 
		ans.push_back(p[i+10]);
	}
    
    return ans; 
}

std::string getprice(std::string line) {
	
	// between the 6th and 7th quotation marks 	
	int quoteindex [7]; 
	int quotecount = 0; 
	
	char p[line.length()];
	
    unsigned int i;
    for (i = 0; i < sizeof(p); i++) {
        p[i] = line[i];
        if ( quotecount < 7 && line[i] == '"') {
			quoteindex[quotecount] = i;
			quotecount++;
		}
    }
	// length of the coin price which is between the 6th and 7th quotation marks 
	int len = quoteindex[6] - quoteindex[5] - 3;
	
	std::string ans; 
	
	for (unsigned int x = 0; x< len; x++) {
		ans.push_back(p[quoteindex[5]+ 2 + x]);
	}
 
	return ans; 
}

bool valdoublein(std::string input) {	
    double d;    
	std::stringstream ss(input);
	if (ss >> d)
	{
		if (ss.eof())
		{
			return true; 
		}
	}        		
    
	std::cout << "Error. Inputted value is not a number." << std::endl;
	return false; 
}

int validateintselection(std::string input, unsigned int length) {
	// returns int selection if valid or -1 if not valid 
	int len = length; 
	int selection;

	std::stringstream ss(input);
	if (ss >> selection) {
		if (ss.eof()) {
			// the input is a valid int , but is it one of the options available?
			for (int i = 0 ; i < len ; i++) {
				if (i == selection) {
					return selection; 
				}
			}
		}
	} // if the input is not a valid int or if it is not one of the options available 
	std::cout << "Error. Invalid input. Please enter a valid selection." << std::endl; 
	return -1; 	
}

double smult (std::string a, std::string b) {
	double x, y;
	std::string::size_type sz;     // alias of size_t
	
	x = stod (a,&sz);
	y = stod (b, &sz); 
	
	return x*y; 
}

double abs(double x) {
	if (x > 0) {
		return x;
	} else {
		return x * -1; 
	}
}

bool appxequal(double x, double y) {
	double diff = x - y;
	if (diff > 0.1 || diff < -0.1) {
		return false;
	} else {
		return true; 
	}
}

void calcusdvalue(Coin &c) {
	c.usdval = smult(c.price, c.quant);
}

void updateusdvalue(Coin(&coins)[10],unsigned int length) {
	for (unsigned int i = 0 ; i < length; i++) {
		calcusdvalue(coins[i]);
	}
}

void readfromfile (std::string filename, Coin (&coins)[10], unsigned int length) {
		
		std::ifstream rfile;
		rfile.open (filename);
	
		for (std::string tp; getline(rfile, tp); ) {
			char p[tp.length()];
			std::string lines [100] ;
			std::string temp = ",";				
			
			int linecount = 0;
			for (int i = 0; i < sizeof(p); i++) {
				p[i] = tp[i];

				temp.push_back(p[i]);
				if (p[i] == 125) {
					lines[linecount] = temp;					
					std::string a = getsym(temp);				
					
					for (unsigned int j = 0; j < length ; j++) {		
						if (a.compare(coins[j].name) == 0) {											
							coins[j].price = getprice(temp); 
							calcusdvalue(coins[j]);						
						}	
					}
					
					temp = "";
					linecount++;			
				}
			}
		}
		rfile.close();		
} 

void printcoin(Coin c) {
	int name_len = c.name.length();
	int price_len = c.price.length();
	int quant_len = c.quant.length();
	std::stringstream stream;
	stream << std::fixed << std::setprecision(6) << c.usdval;
	std::string usdval = stream.str();
	int usdval_len = usdval.length();
	int tbal_len = c.tbal.length(); 

	const int tname_len = 6;
	const int tprice_len = 24;
	const int tquant_len = 20; 
	const int tusdval_len = 14; 
	const int ttbal_len = 6;

	int name_dash = tname_len - name_len; 
	int price_dash = tprice_len - price_len;
	int quant_dash = tquant_len - quant_len;
	int usdval_dash = tusdval_len - usdval_len;
	int tbal_dash = ttbal_len - tbal_len;
	
	std::cout << c.name; 
	for (int a = 0 ; a < name_dash; a++) {
		std::cout << ' ';
	}	

	std::cout << c.price; 
	for (int d = 0 ; d < price_dash; d++) {
		std::cout << ' ';
	}

	std::cout << c.quant; 
	for (int f = 0 ; f < quant_dash; f++) {
		std::cout << ' ';
	}

	std::cout << usdval; 
	for (int h = 0 ; h < usdval_dash; h++) {
		std::cout << ' ';
	}

	std::cout << c.tbal << "%" << std::endl; 
	
}

void print_coins(Coin (&coins)[10], unsigned int length) {
	std::cout << "Name  Price                   Quant               USD Val       T Bal " << std::endl;

	for (unsigned int x = 0 ; x < length; x++) {
		printcoin(coins[x]);
	}

	double totalvalue = 0; 

	for (unsigned int i = 0 ; i < length; i++) {
		totalvalue += coins[i].usdval;	
	}

	std::cout << "Total Value:    " << totalvalue << std::endl;
	std::cout << "-------" << std::endl; 
}

void printacttargdiff(Coin (&coins)[10], unsigned int length, double offset[10], double total, double tbald[10]) {

	// convert to a string rather than printing double directly
	// set precision 
	// add + if positive before $ and - if negative before $
	// set consistent column width 
	std::cout << "Your current holdings compare with your target goal as follows:" << std::endl; 
	double tbal;
	for (unsigned int i = 0; i < length; i++) {
		std::cout << "$" << offset[i] << "USD " << coins[i].name 
		<< " (" << 100*coins[i].usdval/total << "%). " 
		<< "Target: " << total*tbald[i]/100 << "USD" << std::endl;
	}
	std::cout << "------" << std::endl; 
}

void customtrade(Coin(&coins)[10], unsigned int length) {

	std::cout << "----" << std::endl;	
	std::cout << "Which coin would you like to sell?" << std::endl;	
	for (unsigned int i = 0; i < length ; i++) {
		std::cout << i << ". " << coins[i].name << std::endl;
	}
	int sellcoin;
	bool validinput = false; 
	while (validinput == false) {
		std::string temp;
		std::cin >> temp; 
		sellcoin = validateintselection(temp, length);
		if (sellcoin != -1) {
			validinput = true; 
		}
	}
	std::cout << "Which coin would you like to buy?" << std::endl;	
	for (unsigned int j = 0; j < length ; j++) {
		std::cout << j << ". " << coins[j].name << std::endl; 
	}
	int buycoin;
	validinput = false; 
	while (validinput == false) {
		std::string temp;
		std::cin >> temp; 
		buycoin = validateintselection(temp, length);
		if (buycoin != -1) {
			validinput = true; 
		}
	}

	std::cout << "How much " << coins[sellcoin].name << " do you want to sell?" << std::endl;

	bool validsell = false; 
	double dsellq;
	std::string::size_type sz; 
	
	while (validsell == false) {
		std::string sellq;
		std::cin >> sellq; 
		validsell = valdoublein(sellq);
		if (validsell == true) {
			dsellq = stod(sellq, &sz); 
		}
	}
	std::cout << "How much " << coins[buycoin].name << " are you getting in exchange?" << std::endl;

	bool validbuy = false; 
	double dbuyq;
	
	while (validbuy == false) {
		std::string buyq;
		std::cin >> buyq; 
		validbuy = valdoublein(buyq);
		if (validbuy == true) {
			dbuyq = stod(buyq, &sz); 
		}
	}

	double bcb;
	double scb; 
	scb = stod (coins[sellcoin].quant, &sz);
	bcb = stod (coins[buycoin].quant, &sz);

	bcb += dbuyq;
	scb -= dsellq;

	coins[sellcoin].quant = std::to_string(scb);
	coins[buycoin].quant = std::to_string(bcb);

	std::cout << "You chose to sell " << dsellq << coins[sellcoin].name << " for " << dbuyq << coins[buycoin].name << std::endl; 
}

void confirm_trade(double mag, Coin(&coins)[10], unsigned int big, unsigned int small) {
	
	std::string::size_type sz; 
	
	double b_price = stod (coins[big].price, &sz); 
	double s_quant = stod (coins[small].quant, &sz); 
	double b_quant = stod (coins[big].quant, &sz); 
	double b_coins_to_sell = mag / b_price; 

	bool valid = false; 
	std::string temp;

	std::cout << "Sell " << b_coins_to_sell << " " << coins[big].name << std::endl;

	while (valid == false) {
		std::cout << "What amount of " << coins[small].name << " are you getting in exchange?" << std::endl;
		temp = "";
		std::cin >> temp; 
		valid = valdoublein(temp);
	}
	
	double dtemp = stod(temp, &sz);

	b_quant -= b_coins_to_sell;
	s_quant += dtemp;	

	//update coin with string version of new coin balance 
	std::stringstream bigstream;
	std::stringstream smallstream;
	bigstream << std::fixed << std::setprecision(10) << b_quant;
	smallstream << std::fixed << std::setprecision(10) << s_quant;
	coins[big].quant = bigstream.str();
	coins[small].quant = smallstream.str();
}

void update_config(Coin(&coins)[10], unsigned int length) {
	std::ofstream wfile("config.txt", std::ofstream::trunc);

	for (unsigned int i = 0 ; i < length; i++) {
		wfile << coins[i].name << " ";
	}
	wfile << std::endl;

	for (unsigned int i = 0 ; i < length; i++) {
		wfile << coins[i].quant << " ";
	}
	wfile << std::endl;

	for (unsigned int i = 0 ; i < length; i++) {
		wfile << coins[i].tbal << " ";
	}
	wfile << std::endl;

	wfile.close();	
}

void recommend(Coin(&coins)[10], double(&offset)[10], unsigned int length) {
	// test input - all values need to add to 0 
	double sum = 0; 
	
	for (unsigned int i = 0; i< length; i++) {
		sum += offset[i];
		//std::cout << "offset " << i << " " << offset[i] << std::endl; 
		//std::cout << "sum = " << sum << std::endl; 
	}
	if (!appxequal(sum, 0.0)) {
		std::cout << sum << "does not add to 0" << std::endl; 	
		return;
	} 
	 
	unsigned int current_index = 0;
	unsigned int other_index = 1;
	bool positive;
	double small = 0; 

	while (current_index < length) {
	 
	// is current index positive or negative?
	if (offset[current_index] > 0) {
		positive = true;
	} else {
		positive = false;
	}

	std::string in; // user input 

	// what is the smaller magnitude?
	if (abs(offset[current_index]) < abs(offset[other_index])) {
		small = abs(offset[current_index]);
	} else {
		small = abs(offset[other_index]);
	}

	// move smaller magnitude from postive value to negative value
	if (positive == true && offset[other_index] < 0) {
		// curr is positive and other is negative	
		std::cout << "move " << small << "USD from " << coins[current_index].name << "to " << coins[other_index].name << std::endl; 

		offset[current_index] = offset[current_index] - small;
		offset[other_index] = offset[other_index] + small; 

		std::cout << "Would you like to make this trade?" << std::endl; 
		std::cout << "Enter 'y' for yes." << std::endl; 
        
		std::cin >> in; 
		if (in.compare("y") == 0) {
			confirm_trade(small, coins, current_index, other_index);
		}
		
	} 
	if (positive == false && offset[other_index] > 0 ) {
		// curr is negative and other is positive 
		std::cout  << "Move " << small << "USD from " << coins[other_index].name << " to " << coins[current_index].name << std::endl; 
		offset[current_index] = offset[current_index] + small;
		offset[other_index] = offset[other_index] - small; 

		std::cout << "Would you like to make this trade?" << std::endl; 
		std::cout << "Enter 'y' for yes." << std::endl; 
        
		std::cin >> in; 
		if (in.compare("y") == 0) {
			confirm_trade(small, coins, other_index, current_index);
		}
	}

	//increment counters 
	if (appxequal(offset[current_index], 0.0)) {
		current_index ++;
		other_index = 1;
	} else {
		other_index++; 
	}
	if(other_index == current_index) {
		other_index++;
	}
	small = 0.0; 
	in = ""; 

	if (current_index > length || other_index > length) {
		std::cout << "index is greater than length." << std::endl;
		return;
	}

	} // end while loop  
}

void balance(Coin (&coins)[10], unsigned int length) {
	/* Calculate total value
	*  Calculate how current coin balances compares to target
	*  Print results
	*  Prompt user - Recommend trades? Custom Trade? Neither?
	*/ 

	double totalvalue = 0; 
	double usd_offset[10]; // the difference between target balance usd and current value usd for each coin 
	double tbald[10]; // target balance in usd 
	std::string::size_type sz;     // alias of size_t - used to convert from string to double 
	
	for (unsigned int x = 0 ; x < length; x++) {
			totalvalue += coins[x].usdval;	
		}	

	for (unsigned int i = 0 ; i < length; i++) {
		tbald[i] = stod (coins[i].tbal, &sz);
		usd_offset[i] = coins[i].usdval - ((tbald[i]/100) * totalvalue);			
	}	

	// print results 
	printacttargdiff(coins, length, usd_offset, totalvalue, tbald);

	// ask user if they would like trade reccomendations 	
	bool escape = false; 
	while (escape == false) {
		char input; 
		std::cout << "Would you like:" << std::endl; 
		std::cout << "1. Trade recommendations?" << std::endl; 
		std::cout << "2. Custom Trade?" << std::endl; 
		std::cout << "0. Neither. Escape from this menu." << std::endl; 
		std::cout << "Enter '1' for recommendations, '2' for custom, or '0' to skip." << std::endl; 
		
		std::cin >> input; 
		if (input == '1') {
			recommend(coins, usd_offset, length);		
		} else if (input == '2') {
			customtrade(coins, length);
		} else {
			escape = true; 
			return; 
		}
		updateusdvalue(coins,length);
		print_coins(coins,length);
		update_config(coins, length);
	}	
}

bool valtbal(Coin (&coins)[10], unsigned int length) {
	// validate tbal read from config
	// true if valid  - false if invalid 
	// tbal is a percent and needs to add to 100

	double sum = 0 ; 
	double x; 
	std::string::size_type sz; 

	for (unsigned int i = 0 ; i < length; i++)  {
		x = stod (coins[i].tbal,&sz);
		sum += x; 
	}

	if (appxequal(sum, 100.0)) {
		return true;
	} else {
		return false;
	}
}

int main(){
	Coin coins[10] ;

	std::string names[10] = {};
	std::string holdings[10] = {};
	std::string tbal[10] = {}; 
	
	readconfig(names, holdings, tbal); 		

	unsigned int length = 0; 
	for (int i = 0; i < 10; i++) {
		if (names[i].length() > 0 ) {
			length++;
			//std::cout << names[i] << holdings[i] << tbal[i] << std::endl;
		}
	}		

	for (int j=0; j< length; j++) {
		coins[j].name = names[j];	
		coins[j].quant = holdings[j];
		coins[j].tbal = tbal[j]; 
		//std::cout << coins[j].name << coins[j].quant << coins[j].tbal << std::endl;
	}		
	if (valtbal(coins, length) == false) {
		std::cout << "Config percents do not add to 100%" << std::endl; 
		std::cout << "Please check config.txt" << std::endl; 
		return 0; 
	}
	readfromfile("list.txt", coins, length); 
	print_coins(coins, length);
	balance(coins, length);
	
	return 0;
}






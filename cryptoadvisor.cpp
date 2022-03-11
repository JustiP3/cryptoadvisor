#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

/*
 * Program Goals:
 * Read text file with config and current holdings - done 
 * Fetch current crypto prices from api - bash script calls api, writes to text file then this program reads from text file - close enough
 * Calculate current values and compare to config - done 
 * Recommend trades to rebalance per config - done, kind of. recommend trades in terms of the coin at hand rather than USD - almost there 
 * Update config file with new balances if recommended trades were made. 
 * 
 * 
 * Refactor and technical goals:
 * Generalize the solution 
 * Implement linked list to allow variable numbers of coins 
 * 
 * ToDo / left off:
 * clean up main() - move to small helper functions 
 * Recommend trades to rebalance per config - done, kind of. recommend trades in terms of the coin at hand rather than USD - almost there 
 * validate input for response to "would you like reblance recommendations?"
 * Update config file with new balances if recommended trades were made. 
 * create follow up loop after recommendations for user input regarding exact trade value
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


void writetofile(std::string) {
	char data[100];
	// creating the file variable of the fstream data type for writing
	std::fstream wfile;
	// opening the file in both read and write mode
	wfile.open ("demo.txt", std::ios::out| std::ios::in );
	// Asking the user to enter the content
	std::cout<< "Please write the content in the file." << std::endl;
	// Taking the data using getline() function
	std::cin.getline(data, 100);
	// Writing the above content in the file named 'demp.txt'
	wfile<< data << std::endl;
	// closing the file after writing
	wfile.close();
}

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
				// if we hit the space increment entries index and copy temp to 
				// names holdings or tbal based on line number	
				// don't copy the space into the temp string 							
								
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
			} else {
				// as long as this char is not space, copy into the temp string 
				temp.push_back(p[i]); // copy char into the temp string 
			}		
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
							coins[j].usdval = smult(coins[j].price,coins[j].quant); 							
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
	std::cout << c.name << "    " ; 
	std::cout << c.price << "   "; 
	std::cout << c.quant << "   " ; 
	std::cout << c.usdval << "   " ; 
	std::cout << c.tbal << "%" << std::endl; 

}

void confirm_trade(double mag, Coin(&coins)[10], unsigned int big, unsigned int small) {
	// move magnitude from coins[big] to coins[small]
	// confirm exact coin amount
	// recalculate usd amount 
	// get price in double 
	std::string::size_type sz;     // alias of size_t
	
	double s_price = stod (coins[small].price,&sz);
	double b_price = stod (coins[big].price, &sz); 
	double s_quant = stod (coins[small].quant, &sz); 
	double b_quant = stod (coins[big].quant, &sz); 

	std::cout << "small and big quant: " << s_quant << " " << b_quant << std::endl; 

	std::cout << "Sell " << (mag /b_price) << " " << coins[big].name << std::endl;
	std::cout << "What amount of " << coins[small].name << " are you getting in exchange?" << std::endl;
	double temp;
	std::cin >> temp; 
	//validate temp 
	b_quant -= temp;
	s_quant += temp;
	std::cout << "new coin balance: " << std::endl;
	std::cout << coins[small].name << s_quant << std::endl;
	std::cout << coins[big].name << b_quant << std::endl;

	// need to update coin with string version of new coin balance 
}

void update_config(Coin(&coins)[10], unsigned int length) {
	// update config with new coin values 
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
	/*
	 * calc total value
	 * cast tbal to double 
	 * calc offset in percent
	 * */
	double totalvalue = 0; 
	double usd_offset[10]; // the difference between target balance usd and current value usd for each coin 
	double tbald; // target balance in usd 
	std::string::size_type sz;     // alias of size_t - used to convert from string to double 
	
	std::cout << "Name    Price              Quant     USD Val    T Bal" << std::endl;
	
	for (unsigned int x = 0 ; x < length; x++) {
		printcoin(coins[x]);
		totalvalue += coins[x].usdval;	
	}
	
	std::cout << "total val is: " << totalvalue << std::endl;	
	

	for (int i = 0 ; i < length; i++) {
		// convert string target balance to double 
		tbald = stod (coins[i].tbal, &sz);

		// populate usd offset array 
		usd_offset[i] = coins[i].usdval - ((tbald/100) * totalvalue);	

		//print results  
		std::cout << "You are holding $" << usd_offset[i] << "USD " << coins[i].name
		<< " compared to target " << "USD: " << tbald*totalvalue/100 << std::endl;
			
	}	

	// ask user if they would like trade reccomendations 
	std::cout << "Would you like trade recommendations?" << std::endl; 
	std::cout << "(y / n)" << std::endl; 
	char input;
	std::cin >> input; 
	if (input == 'y') {
		recommend(coins, usd_offset, length);
	} else {
		std::cout << "no" << std::endl; 
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

	readfromfile("list.txt", coins, length); 
	
	balance(coins, length);
	
	return 0;
}






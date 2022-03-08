#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

/*
 * Program Goals:
 * Read text file with config and current holdings
 * Fetch current crypto prices from api 
 * Calculate current values and compare to config 
 * Recommend trades to rebalance per config 
 * Update config file with new balances if recommended trades were made. 
 * 
 * 
 * Refactor and technical goals:
 * Generalize the solution 
 * Implement linked list to allow variable numbers of coins 
 * 
 * ToDo / left off:
 * readconfig should error check target balance percents to validate that they add to 100% 
 * validate input for response to "would you like reblance recommendations?"
 * github currently has my api key public , how to pass variables into a shell script? need to fix that  
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
	
	input >> names[0] >> names[1] >> names[2] >> names[3] >> names[4] >> names[5]
	>> holdings[0] >> holdings[1] >> holdings[2] >> holdings[3] >> holdings[4] >> holdings [5]
	>> tbal[0] >> tbal[1] >> tbal[2] >> tbal[3] >> tbal[4] >> tbal[5]; 

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

void readfromfile (std::string filename, Coin (&coins)[10]) {
		
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
					
					for (int j = 0; j < 10 ; j++) {						
						if (coins[j].name.length() > 0 && a == coins[j].name) {				
							coins[j].price = getprice(temp); 
							//cout << "price: " << coins[j].price << " | quant: " << coins[j].quant << std::endl;
							coins[j].usdval = smult(coins[j].price,coins[j].quant); 							
							//cout << coins[j].name << ": " << coins[j].usdval << endl;
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

void recommend(Coin(&coins)[10], double(&offset)[10]) {
	// test input - all values need to add to 0 
	double sum = 0; 
	unsigned int length = 0;
	for (unsigned int j = 0 ; j < 10; j++) {
		if (coins[j].name.length() > 0) {
			length ++; 
		}
	}
	for (unsigned int i = 0; i< length; i++) {
		sum += offset[i];
		std::cout << "offset " << i << " " << offset[i] << std::endl; 
		std::cout << "sum = " << sum << std::endl; 
	}
	if (!appxequal(sum, 0.0)) {
		std::cout << sum << " does not add to 0" << std::endl; 	
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

	// what is the smaller magnitude?
	if (abs(offset[current_index]) < abs(offset[other_index])) {
		small = abs(offset[current_index]);
	} else {
		small = abs(offset[other_index]);
	}

	// move smaller magnitude from postive value to negative value
	if (positive == true && offset[other_index] < 0) {
		// curr is positive and other is negative	
		std::cout << "move " << small << " from curr to other." << std::endl; 
		offset[current_index] = offset[current_index] - small;
		offset[other_index] = offset[other_index] + small; 

	} 
	if (positive == false && offset[other_index] > 0 ) {
		// curr is negative and other is positive 
		std::cout  << "move " << small << " from other to curr." << std::endl; 
		offset[current_index] = offset[current_index] + small;
		offset[other_index] = offset[other_index] - small; 
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

	if (current_index > length || other_index > length) {
		std::cout << "index is greater than length." << std::endl;
		return;
	}

	} // end while loop  
}

void balance(Coin (&coins)[10]) {
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
	
	for (unsigned int x = 0 ; x < 10; x++) {
		if (coins[x].name.length() > 0) {
			printcoin(coins[x]);
			totalvalue += coins[x].usdval;
		}		
	}
	
	std::cout << "total val is: " << totalvalue << std::endl;	
	

	for (int i = 0 ; i < 10; i++) {
		if (coins[i].name.length() > 0) {

			// convert string target balance to double 
			tbald = stod (coins[i].tbal, &sz);

			// populate usd offset array 
			usd_offset[i] = coins[i].usdval - ((tbald/100) * totalvalue);	

			//print results  
			std::cout << "You are holding $" << usd_offset[i] << "USD " << coins[i].name
			<< " compared to target " << "USD: " << tbald*totalvalue/100 << std::endl;
		}		
	}	

	// ask user if they would like trade reccomendations 
	std::cout << "Would you like trade recommendations?" << std::endl; 
	std::cout << "(y / n)" << std::endl; 
	char input;
	std::cin >> input; 
	if (input == 'y') {
		recommend(coins, usd_offset);
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
	
	for (int i=0; i< 6; i++) {
		coins[i].name = names[i];	
		coins[i].quant = holdings[i];
		coins[i].tbal = tbal[i]; 
	}		

	readfromfile("list.txt", coins); 
	
	balance(coins);
	
	return 0;
}






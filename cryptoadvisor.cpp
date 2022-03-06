#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;
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
 * problem in smult - improper index referenced or something  
 * 
 * 
*/ 

struct Coin 
{
	string name;
	string quant; 
	string price; 
	double usdval;
	string tbal;
};


void writetofile(string) {
	char data[100];
	// creating the file variable of the fstream data type for writing
	fstream wfile;
	// opening the file in both read and write mode
	wfile.open ("demo.txt", ios::out| ios::in );
	// Asking the user to enter the content
	cout<< "Please write the content in the file." <<endl;
	// Taking the data using getline() function
	cin.getline(data, 100);
	// Writing the above content in the file named 'demp.txt'
	wfile<< data <<endl;
	// closing the file after writing
	wfile.close();
}

void readconfig(string (&names)[10], string (&holdings)[10], string (&tbal)[10]) {
	ifstream input;
	input.open("config.txt");
	
	input >> names[0] >> names[1] >> names[2] >> names[3] >> names[4] >> names[5]
	>> holdings[0] >> holdings[1] >> holdings[2] >> holdings[3] >> holdings[4] >> holdings [5]
	>> tbal[0] >> tbal[1] >> tbal[2] >> tbal[3] >> tbal[4] >> tbal[5]; 

	input.close(); 	
}

string getsym(string line) {
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
		cout << "error" << endl; 
	}
	string ans;
	for (int i = 0; i < len; i++) {		 
		ans.push_back(p[i+10]);
	}
    
    return ans; 
}

string getprice(string line) {
	
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
	
	string ans; 
	
	for (unsigned int x = 0; x< len; x++) {
		ans.push_back(p[quoteindex[5]+ 2 + x]);
	}
 
	return ans; 
}

double smult (string a, string b) {
	double x, y;
	string::size_type sz;     // alias of size_t
	
	x = stod (a,&sz);
	y = stod (b, &sz); 
	
	return x*y; 
}

void readfromfile (string filename, Coin (&coins)[10]) {
		
		ifstream rfile;
		rfile.open (filename);
	
		for (string tp; getline(rfile, tp); ) {
			char p[tp.length()];
			string lines [100] ;
			string temp = ",";				
			
			int linecount = 0;
			for (int i = 0; i < sizeof(p); i++) {
				p[i] = tp[i];

				temp.push_back(p[i]);
				if (p[i] == 125) {
					lines[linecount] = temp;					
					string a = getsym(temp);					
					
					for (int j = 0; j < 10 ; j++) {						
						if (coins[j].name.length() > 0 && a == coins[j].name) {				
							coins[j].price = getprice(temp); 
							cout << "price: " << coins[j].price << " | quant: " << coins[j].quant << endl;
							coins[j].usdval = smult(coins[j].price,coins[j].quant); 							
							cout << coins[j].name << ": " << coins[j].usdval << endl;
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
	cout << c.name << "    " ; 
	cout << c.price << "   "; 
	cout << c.quant << "   " ; 
	cout << c.usdval << "   " ; 
	cout << c.tbal << "%" << endl; 

}

void balance(Coin (&coins)[10]) {
	/*
	 * calc total value
	 * cast tbal to double 
	 * calc offset in percent
	 * */
	double totalvalue = 0; 
	double usd_offset[10];
	double tbald; 
	
	cout << "Name    Price              Quant     USD Val    T Bal" << endl;
	for (int i = 0 ; i < 6; i++) {
		printcoin(coins[i]);
		totalvalue += coins[i].usdval; 
	}
	cout << "total val is: " << totalvalue << endl;
	
	
	for (int i = 0 ; i < 6; i++) {
		char a[2];
	
		for (int b=0; b<2;b++) {
			a[b] = coins[i].tbal[b];
		}
		tbald = atof(a);
		usd_offset[i] = coins[i].usdval - ((tbald/100) * totalvalue);
		//offset[i] =(coins[i].usdval / totalvalue) - (tbald/100) ;		 
		cout << "You are holding $" << usd_offset[i] << "USD " << coins[i].name
		<< " compared to target " << "USD: " << tbald*totalvalue/100 << endl;
		//cout << "Target USD Value is: " << (tbald/100) * totalvalue << endl;		
	}
	
}




int main(){
	Coin coins[10] ;

	string names[10] = {};
	string holdings[10] = {};
	string tbal[10] = {}; 
	
	readconfig(names, holdings, tbal); 		
	
	for (int i=0; i< 6; i++) {
		coins[i].name = names[i];	
		coins[i].quant = holdings[i];
		coins[i].tbal = tbal[i]; 
	}		

	readfromfile("list.txt", coins); 
	
	//balance(coins);
	
	return 0;
}






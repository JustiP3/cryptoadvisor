# Crypto Advisor 

This is a program for someone who holds multiple cryptocurrencies and wants to maintain a specific set balance. 
For example a user might hold three currencies: BTC, ETH, and USDT. They may want to always hold 40% of the overall value in BTC, 40% in ETH, and 20% in USDT.
This program will help the user by telling them:
1. The current overall wallet balance. 
2. How the value is distributed. 
3. Recommend exchanges to meet the desired target balance. 

Program Goals:
Read config.txt - a user provided file. 
Fetch current crypto prices from api. 
Calculate current values and compare to config values. 
Recommend trades to rebalance per config. 
Update config file with new balances assuming trades were made as recommended.


## Config File
The config file is where the user will define:
1. Which coins they are interested in. 
2. How much of each coin they are holding. 
3. The target balance of each coin as a percent. 

The format must be as follows:
1. Line 1: Coin symbols seperated by a space
2. Line 2: How much of each coin you hold in the same order as line 1. 
3. Target balance for each coin as a percent in the same order. 

----config.txt----<br>
BTC ETC USDT <br>
0.001 0.3 100 <br>
40 40 20 <br>
----END----

In the example above the user is indicating that they currently hold 0.001 BTC, 0.3 ETC, and 100 USDT. 
They would like their overall balance to be 40% BTC, 40% ETC and 20% USDT. 

## Why
Any programmer looking at this might ask, why C++?
Good question. C++ definitely does not feel like the right tool for the job if you ask me. I just want to get better at programming in C and C++ and I had an idea for a program so I decide to try it out even though it would have probably been easier for me in javascript or python. 
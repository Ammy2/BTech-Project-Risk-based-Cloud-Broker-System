# B-Tech Project: Risk Based Cloud Broker System

### Team: Aman Arora and Pulkit Agarwal
### Mentor: Prof. Ganesh Iyer

Simulation and Analysis for Risk Based Cloud Broker System 

A cloud broker is a third-party individual or business that acts as an intermediary between the purchaser of a cloud computing service and the sellers of that service. In general, a broker is someone who acts as an intermediary between two or more parties during negotiations.

User have its own utility funtion which are dependent on :

1) Trust with reference
2) Trust without refernce

CSP follows a dynamminc pricing strategy in order to decide what price to offer for a particular resource to a particular use. This dynamminc pricing startegy was based on following parameters :

1) Acceptance ratio
2) Resource popularity
3) Average price of the resource
4) Reputation value of the CSP

Exact mathematical formulas and relations are mentioned in the Project Report.

*Running the code*

Just compile and run the C++ code in the src folder 
1) g++ main.cpp
2) ./a.out

*Structure of the code*

Class CSP - This basically stores data related to a particular CSP
Class Collective_CSP - This class basically act as a broker and stores data collectively about all the CSPs.
Struct User - Data pertaining to a particular user.


Code is very well documented with comments specifying the role of each of the functions.
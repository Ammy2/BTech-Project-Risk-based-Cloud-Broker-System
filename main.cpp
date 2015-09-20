#include<stdio.h>
#include<stdlib.h>
#include<vector>
#define n_users 10
#define n_resources 5
#define n_csp 5
using namespace std;

class Collective_CSP {
private:
	vector<int> popularity(n_resource,0);
	vector<vector<double> > avg_price(n_csp, vector<double>(n_resource,0.0));
	vector<double> reputation(n_csp,0.0);
public:
	Collective_CSP(){
		for(int i=0; i<n_resource; i++){
			popularity[i] = rand()%90+10; //no. of times resource requested is between 10->100 initially
		}
		for(int i=0; i<n_csp; i++){
			reputation[i] = rand();
		}
	}
	void update_csp_manager(int csp_no, CSP csp){
		for(int i=0; i<n_resource; i++){
			avg_price[csp_no][i] = csp.getAvgPrice(i);
		}
	}
	int getResourcePopularity(int res_no){
		return popularity[res_no];
	}
	void setResourcePopularity(int res_no, int popularity){
		//TODO
	}

	double getMarketCompetition(int res_no){
		double avg;
		for(int i=0; i<n_csp; i++){
			avg += reputation[i]*avg_price[i][res_no];
		}
		return avg;
	}

	double getAveragePriceResource(int res_no){
		double sum = 0.0;
		for(int i=0; i<n_csp; i++){
			sum += avg_price[i][res_no];
		}
		return sum/(double)n_csp;
	}

	double getReputation(int csp_no){
		return reputation[csp_no];
	}

	//TODO
	void setReputation(int csp_no, double reputation){

	}
};

class CSP {
private:
	vector<vector<double> > user_res_price(n_users, vector<double>(n_resource,0.0));
	double reputation_th = 0.0;
	double acceptance_rate = 0.0;
public:
	CSP(){
		for(int i=0; i<n_users; i++){
			for(int j=0; j<n_resource; j++){
				user_res_price[i][j] = rand()%40+10;
			}
		}
		reputation_th = rand();
		acceptance_rate = rand();
	}
	//TODO
	double setThresholdReputation(){

	}

	double getThresholdRep(){
		return reputation_th;
	}

	double getAcceptanceRate(){
		return acceptance_rate;
	}
	//TODO
	double setAcceptanceRate(){

	}

	double getPrice(int user, int resource){
		return user_res_price[user][resource];
	}

	void setUserResPrice(double price, int user_no, res_no){
		user_res_price[user_no][res_no] = price;
	}

	double getAvgPrice(int res_no){
		double sum = 0.0;
		for(int i=0; i<n_users; i++){
			sum += user_res_price[i][res_no];
		}
		return sum/(double)n_users;
	}
};

// Dynammic pricing strategy follows. 
//Following code will return the price CSP i will be offering for resource j,(Rij).
double getDynamicPrice(int csp, int resource, int user, vector<CSP> & csps, Collective_CSP csp_manager){

	int resouce_popularity = csp_manager.getResourcePopularity(resource);
	double avg_price_resource = csp_manager.getMarketCompetition(resource);
	double avg_market_price = csp_manager.getAveragePriceResource(resource);
	double previous_price = csps[csp].getPrice(user, resource);
	double threshold_rep = csps[csp].getThresholdRep();
	double current_rep = csp_manager.getReputation(csp);
	double acceptance_rate = csps[i].getAcceptanceRate();
	double Pij = some_function(); //TODO
	return Pij;
}

int main(){
	Collective_CSP csp_manager;
	vector<CSP> csps;
	for(int i=0; i<n_csp; i++){
		csps.push_back(new CSP());
		csp_manager.update_csp_manager(i,csps[i]);
	}
	// Amans portion. Implementing users utility function.

	double price = getDynamicPrice(csp, resource, user, csps, csp_manager);
	return 0;
}
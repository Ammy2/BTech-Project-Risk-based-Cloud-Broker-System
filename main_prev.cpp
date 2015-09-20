#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<limits.h>
#define n_users 10
#define n_resource 5
#define n_csp 5
#define min_jr 20					// JR to be divided by 100, [0,1]
#define max_jr 90
#define n_iterations 10000
#define freq 100
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

typedef struct user{
	double risk_lambda;
	double budget;
	vector<double> job_rating;	// for all CSP
	vector<double> local_trust;
	vector<double> ref_credit;  // for other reference users
	vector<double> ref_trust;	// for all CSP
	vector<double> util_res;	// A[i] denotes the csp index with max utility, for i'th resource
}user;
user *users;
double alpha;
void user_initialize(){
	users = new user[n_users];
	for(int i=0;i<n_users;i++){
		users[i].risk_lambda = rand()%4 -2;	// [-2,2]
		for(int j=0;j<n_csp; j++){
			double temp = (double)(((rand()%(max_jr-min_jr)) + min_jr)/100.00);	//[0,1]
			users[i].job_rating.push_back(temp);
		}
		for(int res=0; res<n_resource; res++){
			users[i].util_res.push_back(INT_MIN);
		}
	}
	alpha = (double)((rand()%10)*0.1);
}
void interations(){
	for(int iter=1;iter<=n_iterations;iter++){
		if(iter%freq==0){
			// call dynamic pricing. Update respected things
		}
		// Using previous Job Ratings to update things for users
		for(int u=0;u<n_users;u++){
			// reset user's utility
			for(int res=0;res<n_resource;res++)
				users[i].util_res[res] = INT_MIN;
			// for every csp
			for( int c=0; c<n_csp; c++){
				users[u].local_trust = getLocalTrust(u,c,iter);
				users[u].ref_credit = getReferenceCredit(u,c,iter);
				users[u].ref_trust = getReferenceTrust(u,c,iter);
				for(int res=0; res<n_resource; res++){
					double utility = computeUtility(u,c,res,iter);
					if(utility> users[u].util_res[res]){
						// u-user finds resouce-res by csp c best!
						users[u].util_res[res] = c;
					}
				}
			}
			// User decided, which csp to work with.
			// users[u].util_res[res] contains the chosen csp index.
		}

	}
}

int main(){
	Collective_CSP csp_manager;
	vector<CSP> csps;
	for(int i=0; i<n_csp; i++){
		csps.push_back(new CSP());
		csp_manager.update_csp_manager(i,csps[i]);
	}
	// Aman's portion. Implementing users utility function.
	user_initialize();
	interations();

	double price = getDynamicPrice(csp, resource, user, csps, csp_manager);
	return 0;
}

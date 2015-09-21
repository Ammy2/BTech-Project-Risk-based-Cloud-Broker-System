#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<limits.h>
#include<math.h>
using namespace std;

const int n_users = 10; 
const int n_resource = 5; 
const int n_csp = 5;
const int min_jr = 20;					// JR to be divided by 100, [0,1]
const int max_jr = 90;
const int n_iterations = 10000;
const int freq = 100;
const int min_budget = 10;
const int max_budget = 100;

class CSP {
private:
	double user_res_price[n_users][n_resource];
	double reputation_th;
	double acceptance_rate;
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

	void setUserResPrice(double price, int user_no, int res_no){
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


class Collective_CSP {
private:
	int popularity[n_resource];
	double avg_price[n_csp][n_resource];
	double reputation[n_csp];
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
			avg += reputation[i] * avg_price[i][res_no];
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

void updateData(int user, int csp, int resource, double price,vector<CSP> & csps, Collective_CSP & csp_manager){
	csps[csp].setUserResPrice(price, user, resource);
	csp_manager.update_csp_manager(csp,csps[csp]);
}

// Dynammic pricing strategy follows. 
//Following code will return the price CSP i will be offering for resource j,(Rij).
double getDynamicPrice(int csp, int resource, int user, vector<CSP> & csps, Collective_CSP & csp_manager){

	int resouce_popularity = csp_manager.getResourcePopularity(resource);
	double avg_price_resource = csp_manager.getMarketCompetition(resource);
	double avg_market_price = csp_manager.getAveragePriceResource(resource);
	double previous_price = csps[csp].getPrice(user, resource);
	double threshold_rep = csps[csp].getThresholdRep();
	double current_rep = csp_manager.getReputation(csp);
	double acceptance_rate = csps[csp].getAcceptanceRate();
	double Pij = previous_price + ((avg_price_resource-previous_price)*exp(0.2*(current_rep-threshold_rep)))/resouce_popularity;
	return Pij;
}


typedef struct user{
	double risk_lambda;
	double budget;
	vector< vector<double> > job_rating;	// for all CSP in a particular iterations
	vector<double> local_trust;
	vector<double> ref_credit;  // for other reference users
	vector<double> ref_trust;	// for all CSP
	vector<double> util_res;	// A[i] denotes the csp index with max utility, for i'th resource
	user(): local_trust(n_csp, 0), ref_credit(n_users, 0), ref_trust(n_csp, 0), util_res(n_resource, INT_MIN)
	{}
}user;
user *users;
double alpha;
void user_initialize(){
	users = new user[n_users];
	for(int i=0;i<n_users;i++){
		users[i].risk_lambda = rand()%4 -2;	// [-2,2]
		vector<double> jratings;
		// for all csp
		for(int j=0;j<n_csp; j++){
			double temp = (double)(((rand()%(max_jr-min_jr)) + min_jr)/100.00);	//[0,1]
			jratings.push_back(temp);
		}
		// 0'th iteration's rating stored
		users[i].job_rating.push_back(jratings);
		users[i].budget = (double)(1.00 * (rand()%(max_budget-min_budget))) + 1.00*min_budget;
	}
	alpha = (double)((rand()%10)*0.1);
}
double getLocalTrust(int uid, int cid, int iter){
	int n = 1;
	double num=0.0,den=0.0;
	while(n<=iter){
		num += pow(alpha, n) * users[uid].job_rating[iter-1][cid];
		den += pow(alpha, n);
		n++;
	}
	return (double)(num/den);
}
void updateReferenceCredit(int uid, int cid, int iter){
	for(int other=0; other<n_users; other++){
		if(uid==other)
			continue;
		double num=0.0;
		for(int c=0;c<n_csp;c++){
			num += fabs( users[uid].job_rating[iter-1][c] - users[other].local_trust[c]);
		}
		users[uid].ref_credit[other] = 1.00 - (double)(num/n_csp);
	}
}
double getReferenceTrust(int uid, int cid, int iter){
	double num = users[uid].local_trust[cid];
	double den = 1.00;
	for(int other=0; other<n_users; other++){
		if(other==uid)
			continue;
		num += (users[uid].ref_credit[other] * users[other].local_trust[cid]);
		den += users[uid].ref_credit[other];
	}
	return (double)(num/den);
}
double computeUtility(int uid, int cid, int res, int iter){
	double price_res_cid;		// price offered by csp cid of resource res at iter
	double exp_cost = price_res_cid / users[uid].budget;
	double utility = ( 1 - exp(-1.0*users[uid].risk_lambda*users[uid].ref_trust[cid])) / (1-exp(-1.0*users[uid].risk_lambda));
	utility /= exp_cost;
	return utility;
}
void updateJobRatings(int uid, int iter){
	vector<double> jratings = users[uid].job_rating[iter-1];
	for(int c=0;c<n_csp;c++){
		if(jratings[c] < 0.98)
			jratings[c]+= 0.01;
	}
	users[uid].job_rating.push_back(jratings);
}
void interations(){
	for(int iter=1;iter<=n_iterations;iter++){
		// NOTE: user user_rating of iter-1'th index
		if(iter%freq==0){
			// call dynamic pricing. Update respected things
		}
		// Using previous Job Ratings to update things for users
		for(int u=0;u<n_users;u++){
			// reset user's utility
			for(int res=0;res<n_resource;res++)
				users[u].util_res[res] = INT_MIN;
			// for every csp
			for( int c=0; c<n_csp; c++){
				users[u].local_trust[c] = getLocalTrust(u,c,iter);
				updateReferenceCredit(u,c,iter);
				users[u].ref_trust[c] = getReferenceTrust(u,c,iter);
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

			// Now Update new Job Ratings
			updateJobRatings(u, iter);
		}
	}
}

int main(){
	Collective_CSP csp_manager;
	vector<CSP> csps;
	for(int i=0; i<n_csp; i++){
		CSP csp;
		csps.push_back(csp);
		csp_manager.update_csp_manager(i,csps[i]);
	}
	// Aman's portion. Implementing users utility function.
	user_initialize();
	interations();
	return 0;
}
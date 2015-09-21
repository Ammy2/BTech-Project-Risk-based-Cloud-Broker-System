#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<limits.h>
#include<math.h>
#include<iostream>
using namespace std;

const int n_users = 10; 
const int n_resource = 5; 
const int n_csp = 5;
const int min_jr = 40;					// JR to be divided by 100, [0,1]
const int max_jr = 50;
const int n_iterations = 1000;
const int freq = 100;
const int min_budget = 50;
const int max_budget = 55;
const int max_repo = 60;
const int min_repo = 40;


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
		reputation_th = (double)(rand()%(max_repo-min_repo) + min_repo)/100.00;
		acceptance_rate = rand();
	}
	void printData(int csp_no){
		cout<<"Inside CSP:"<<csp_no<<endl;
		cout<<"Printing Reputation Threshold: ";
		cout<<reputation_th<<endl;
		cout<<"Printing Acceptance Rate Data";
		cout<<acceptance_rate<<endl;
		cout<<"Printing User Resource Price Data"<<endl;
		for(int i=0; i<n_users; i++){
			for(int j=0; j<n_resource; j++){
				cout<<user_res_price[i][j]<<" ";
			}
			cout<<endl;
		}
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
		//cout << user<<" "<<resource<<endl;
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
			reputation[i] = (double)(rand()%(max_repo-min_repo) + min_repo)/100.00;
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
	void setResourcePopularity(int res_no){
		popularity[res_no] ++;
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

	void printData(){
		cout<<"Inside Collective CSP:"<<endl;
		cout<<"Printing Populatity Data: ";
		for(int i=0; i<n_resource; i++){
			cout<<popularity[i]<<" ";
		}
		cout<<endl;
		cout<<"Printing Reputation Data: ";
		for(int i=0; i<n_csp; i++){
			cout<<reputation[i]<<" ";
		}
		cout<<endl;
		cout<<"Printing Avg Price Data"<<endl;
		for(int i=0; i<n_csp; i++){
			for(int j=0; j<n_resource; j++){
				cout<<avg_price[i][j]<<" ";
			}
			cout<<endl;
		}
	}
};

Collective_CSP csp_manager;
vector<CSP> csps;

void updateData(int user, int csp, int resource, double price,vector<CSP> & csps, Collective_CSP & csp_manager){
	csps[csp].setUserResPrice(price, user, resource);
	csp_manager.update_csp_manager(csp,csps[csp]);
	csp_manager.setResourcePopularity(resource);
}

// Dynammic pricing strategy follows. 
//Following code will return the price CSP i will be offering for resource j,(Rij).
double getDynamicPrice(int csp, int resource, int user, vector<CSP> & csps, Collective_CSP & csp_manager){

	int resouce_popularity = csp_manager.getResourcePopularity(resource);
	double avg_price_resource = csp_manager.getMarketCompetition(resource);
	//cout<<"AVG Price->"<<avg_price_resource<<endl;
	double avg_market_price = csp_manager.getAveragePriceResource(resource);
	//cout<<"AVG Market Price->"<<avg_market_price<<endl;
	double previous_price = csps[csp].getPrice(user, resource);
	double threshold_rep = csps[csp].getThresholdRep();
	//cout<<"Th Repo->"<<threshold_rep<<endl;
	double current_rep = csp_manager.getReputation(csp);
	//cout<<"Current Repo->"<<current_rep<<endl;
	double acceptance_rate = csps[csp].getAcceptanceRate();
	double Pij = previous_price + ((avg_price_resource-previous_price)*exp(0.2*(current_rep-threshold_rep)))/resouce_popularity;
	//cout<<"Test->"<<((avg_price_resource-previous_price)*exp(0.2*(current_rep-threshold_rep)))/resouce_popularity<<endl;
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

void print_initial_data(){
	cout<<" #Users: "<<n_users<<endl;
	cout<<" #CSPs: "<<n_csp<<endl;
	cout<<" #Resources: "<<n_resource<<endl;
	cout<<" min_jr: "<<min_jr/100.0<<endl;
	cout<<" max_jr: "<<max_jr/100.0<<endl;
	cout<<" min_budget: "<<min_budget<<endl;
	cout<<" max_budget: "<<max_budget<<endl;
	cout<<" min_repo: "<<min_repo/100.0<<endl;
	cout<<" max_repo: "<<max_repo/100.0<<endl;
	cout<<" n_iterations: "<<n_iterations<<endl;
	cout<<" freq: "<<freq<<endl;
	cout<<" Alpha: "<<alpha<<endl;
	for(int u=0;u<n_users;u++){
		cout<<" User: "<<u<<endl;
		cout<<" Risk Lambda: "<<users[u].risk_lambda<<endl;
		cout<<" Budget: "<<users[u].budget<<endl;
		cout<<" Initial Job Ratings: "<<endl;
		for(int c=0;c<n_csp;c++){
			cout<<"  CSP: "<<c<<" JRating: "<<users[u].job_rating[0][c]<<endl;
		}
	}
}
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
	alpha = 0.83;
	print_initial_data();
}
void updateLocalTrust(int iter){
	for(int uid=0;uid<n_users;uid++){
		for(int cid=0;cid<n_csp;cid++){
			int n = 1;
			double num=0.0,den=0.0;
			int tt = iter-1;
			while(n<=iter){
				num += pow(alpha, n) * users[uid].job_rating[tt][cid];
				den += pow(alpha, n);
				n++;
				tt--;
			}
			users[uid].local_trust[cid]=(double)(num/den);
		}
	}
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
		num += (users[uid].ref_credit[other]) * (users[other].local_trust[cid]);
		den += users[uid].ref_credit[other];
	}
	return (num/(double)den);
}
double computeUtility(int uid, int cid, int res, int iter){
	double price_res_cid = csps[cid].getPrice(uid, res);		// price offered by csp cid of resource res at iter
	
	double exp_cost = price_res_cid / users[uid].budget;
	
	double a = users[uid].risk_lambda;
	double b = users[uid].ref_trust[cid];
	double top = -1.0*(users[uid].ref_trust[cid]);
	
	double utility = ( 1.0 - exp(top)) / (1.0-exp(-1.0*users[uid].risk_lambda));

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
	vector<double> revenue(n_csp, 0.00);
	for(int iter=1;iter<=n_iterations;iter++){
		//cout << "Iteration : "<<iter<<endl;
		// NOTE: user user_rating of iter-1'th index
		if(iter%freq==0){
			// Fetch Points {iter: [revenues for csps]} 
			//cout <<"I"<<iter<<endl;
			for(int c=0;c<n_csp;c++)
				cout<<revenue[c]<<" ";
			cout<<endl;
		}
		// Using previous Job Ratings to update things for users
		updateLocalTrust(iter);
		for(int u=0;u<n_users;u++){
			// reset user's utility
			for(int res=0;res<n_resource;res++)
				users[u].util_res[res] = INT_MIN;
			// for every csp
			for( int c=0; c<n_csp; c++){
				updateReferenceCredit(u,c,iter);
				users[u].ref_trust[c] = getReferenceTrust(u,c,iter);
				for(int res=0; res<n_resource; res++){
					double utility = computeUtility(u,c,res,iter);
					//cout<<"U "<<utility<<endl;
					if(utility> users[u].util_res[res]){
						// u-user finds resouce-res by csp c best!
						users[u].util_res[res] = c;
					}
				}
			}
			// User decided, which csp to work with.
			// users[u].util_res[res] contains the chosen csp index.
			for(int res=0;res<n_resource;res++){
				int chosen_csp = users[u].util_res[res];
				revenue[chosen_csp] += csps[chosen_csp].getPrice(u, res);
			}

			// Now Update new Job Ratings
			updateJobRatings(u, iter);

			// Apply dynamic pricing
			for(int c=0;c<n_csp;c++)
				for(int res=0;res<n_resource;res++)
					updateData(u,c,res, getDynamicPrice(c,res,u,csps,csp_manager), csps, csp_manager);
		}
	}
}



int main(){
	for(int i=0; i<n_csp; i++){
		CSP csp;
		csps.push_back(csp);
		//csps[i].printData(i);
		csp_manager.update_csp_manager(i,csps[i]);
	}
	//csp_manager.printData();
	// double price;
	// price = getDynamicPrice(1,1,3,csps, csp_manager);
	// cout<<"Price->"<<price<<endl;
	// price = getDynamicPrice(2,4,7,csps, csp_manager);
	// cout<<"Price->"<<price<<endl;
	// price = getDynamicPrice(3,0,6,csps, csp_manager);
	// cout<<"Price->"<<price<<endl;	// Aman's portion. Implementing users utility function.

	user_initialize();
	interations();
	cout<<"Prices"<<endl;
	for(int c=0;c<n_csp;c++){
		cout<<" CSP"<<c<<endl;
		for(int u=0;u<n_users;u++){
			cout<<"  User"<<u<<endl;
			cout<<"   ";
			for(int res=0;res<n_resource;res++){
				cout<<csps[c].getPrice(u, res)<<" ";
			}
			cout<<endl;
		}
	}
	return 0;
}
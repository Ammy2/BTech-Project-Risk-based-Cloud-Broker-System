#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<limits.h>
#include<math.h>
#include <time.h>
#include<iostream>
using namespace std;

const int n_users = 5; 
const int n_resource = 5; 
const int n_csp = 5;
const int min_jr = 50;					// JR to be divided by 100, [0,1]
const int max_jr = 75;
const int n_iterations = 1000;
const int freq = 100;
const int min_budget = 40;
const int max_budget = 100;
const int max_repo = 55;
const int min_repo = 50;
const int max_acc = 70;
const int min_acc = 50;
const int min_acc_th = 50;
const int max_acc_th = 70;
const int min_price = 5;
const int max_price = 20;
const double epsilon = 0.4;
const double rho = 0.5;


class CSP {
private:
	double user_res_price[n_users][n_resource];
	double reputation_th;
	double acceptance_th;
public:
	CSP(){
		for(int i=0; i<n_users; i++){
			for(int j=0; j<n_resource; j++){
				user_res_price[i][j] = rand()%(max_price-min_price)+min_price;
			}
		}
		reputation_th = (double)(rand()%(max_repo-min_repo) + min_repo)/100.00;
		acceptance_th =  (double)(rand()%(max_acc_th-min_acc_th) + min_acc_th)/100.00;
	}
	void printData(int csp_no){
		cout<<"Inside CSP:"<<csp_no<<endl;
		cout<<"Printing Reputation Threshold: ";
		cout<<reputation_th<<endl;
		cout<<"Printing Acceptance Rate Threshold";
		cout<<acceptance_th<<endl;
		cout<<"Printing User Resource Price Data"<<endl;
		for(int i=0; i<n_users; i++){
			for(int j=0; j<n_resource; j++){
				cout<<user_res_price[i][j]<<" ";
			}
			cout<<endl;
		}
	}

	/*
	Currently Threshold reputation is a static entity and remain constant throughout. Given a random value.
	double setThresholdReputation(){

	}	
	*/

	double getThresholdRep(){
		return reputation_th;
	}

	double getThresholdAcceptance(){
		return acceptance_th;
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
	int accepted_resources[n_csp]; //to be incremented only when a particular CSP's resource gets accepted.
public:
	Collective_CSP(){
		for(int i=0; i<n_resource; i++){
			popularity[i] = rand()%10; //no. of times resource requested is between 0->10 initially
		}
		for(int i=0; i<n_csp; i++){
			reputation[i] = (double)(rand()%(max_repo-min_repo) + min_repo)/100.00;
		}
		for(int i=0; i<n_csp; i++){
			accepted_resources[i] = (double)(rand()%(max_acc-min_acc) + min_acc);
		}
	}
	void update_csp_manager(int csp_no, CSP &csp){
		for(int i=0; i<n_resource; i++){
			avg_price[csp_no][i] = csp.getAvgPrice(i);
		}
	}
	double getAcceptanceRate(int iteration_no, int csp){
		return accepted_resources[csp]/(double)(iteration_no*n_users*n_resource);
	}
	
	int getAcceptedResource(int csp){
		return accepted_resources[csp];
	}

	void setAcceptedResource(int csp){ // need to called when the resouce for a particular CSP gets accepted
		accepted_resources[csp] ++;
	}

	int getResourcePopularity(int res_no){
		return popularity[res_no];
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
	//csp_manager.setResourcePopularity(resource);
}

// Dynammic pricing strategy follows. 
//Following code will return the price CSP i will be offering for resource j,(Rij).

//---Need to pass iteration_no in this function //
double getDynamicPrice(int csp, int resource, int user, vector<CSP> & csps, Collective_CSP & csp_manager, int iteration_no){

	int resouce_popularity = csp_manager.getResourcePopularity(resource);
	double avg_price_resource = csp_manager.getMarketCompetition(resource);
	double previous_price = csps[csp].getPrice(user, resource);
	double threshold_rep = csps[csp].getThresholdRep();
	double current_rep = csp_manager.getReputation(csp);
	double acceptance_rate = csp_manager.getAcceptanceRate(iteration_no, csp);
	double acceptance_th = csps[csp].getThresholdAcceptance();
	double offset = ((acceptance_rate-acceptance_th) *(avg_price_resource+previous_price)/2
									*exp(rho*(current_rep-threshold_rep)))/resouce_popularity;
	double Pij = previous_price + offset;
	// cout<<" csp: "<<csp<<" res: "<<resource<<" u:"<<user<<" iter:"<<iteration_no;
	// cout<<" f:"<<(acceptance_rate-acceptance_th)<<" s:"<<(avg_price_resource+previous_price);
	// cout<<" t:"<<(acceptance_rate-acceptance_th) *(avg_price_resource+previous_price)/2<<" fo:"<<exp(0.2*(current_rep-threshold_rep));
	// cout<<" fi:"<<((acceptance_rate-acceptance_th) *(avg_price_resource+previous_price)/2
	// 								*exp(0.2*(current_rep-threshold_rep)));
	// cout<<" si:"<<resouce_popularity<<" off:"<<((acceptance_rate-acceptance_th) *(avg_price_resource+previous_price)
	// 								*exp(0.2*(current_rep-threshold_rep)))/resouce_popularity;
	// cout<<" pt-1:"<<previous_price;
	// cout<<endl;
	if(fabs(offset) > (previous_price/3.0)){
		if(offset>0)
			Pij = 4.0*previous_price/3.00;
		else
			Pij = 2.0*previous_price/3.00;
	}

	return Pij;
}

double getDynamicPrice_gi(int csp, int resource, int user, vector<CSP> & csps, Collective_CSP & csp_manager, int iteration_no){

	double previous_price = csps[csp].getPrice(user, resource);
	double acceptance_rate = csp_manager.getAcceptanceRate(iteration_no, csp);
	double acceptance_th = csps[csp].getThresholdAcceptance();
	double avg_market_price = csp_manager.getAveragePriceResource(resource);
	double Pij = previous_price + 
				(acceptance_rate - acceptance_th)*(epsilon*previous_price + (1-epsilon)*avg_market_price);

	double offset = (acceptance_rate - acceptance_th)*(epsilon*previous_price + (1-epsilon)*avg_market_price);

	// cout<<" csp: "<<csp<<" res: "<<resource<<" u:"<<user<<" iter:"<<iteration_no;
	// cout<<" f:"<<(acceptance_rate-acceptance_th)<<" s:"<<((1-epsilon)*avg_market_price+epsilon*previous_price);
	// cout<<" offset:"<<offset<<" prev:"<<previous_price<<endl;

	if(fabs(offset) > (previous_price/3.0)){
		if(offset>0)
			Pij = 4.0*previous_price/3.00;
		else
			Pij = 2.0*previous_price/3.00;
	}
	return Pij;
}

typedef struct user{
	double risk_lambda;
	double budget;
	vector< vector<double> > job_rating;	// for all CSP in a particular iterations
	vector<double> local_trust;
	vector<double> ref_credit;  // for other reference users
	vector<double> ref_trust;	// for all CSP
	vector<pair<double, int> > util_res;	// A[i] denotes the csp index along with max utility, for i'th resource
	user(): local_trust(n_csp, 0), ref_credit(n_users, 0), ref_trust(n_csp, 0), util_res(n_resource, make_pair(INT_MIN, -1))
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
	double top = -1.0*(a*b);
	
	double utility = ( 1.0 - exp(top)) / (1.0-exp(-1.0*a));
	if(a==0)
		utility = 1;

	utility /= exp_cost;
	//cout<< " Utility: "<<utility<<" u:"<<uid<<" cid"<<cid<<" res:"<<res<<" iter:"<<iter<<" price:"<<price_res_cid<<endl;
	return utility;
}
void updateJobRatings(int uid, int iter){
	vector<double> jratings = users[uid].job_rating[iter-1];
	for(int c=0;c<n_csp;c++){
		if(jratings[c] < 0.98)
			jratings[c]+= 0.001;
	}
	users[uid].job_rating.push_back(jratings);
}
void updatestaticJobRatings(int uid, int iter){
	vector<double> jratings = users[uid].job_rating[iter-1];
	// for(int c=0;c<n_csp;c++){
	// 	if(jratings[c] < 0.98)
	// 		jratings[c]+= 0.01;
	// }
	users[uid].job_rating.push_back(jratings);
}
double find_jain(vector<double> &revenue){
	double num_sum=0;
	double den_sum = 0;
	for(int i=0;i<n_csp;i++){
		num_sum+=revenue[i];
		den_sum+=pow(revenue[i],2);
	}
	num_sum = pow(num_sum,2);
	return num_sum/(double)(n_csp*den_sum); 
}
void interations(){
	vector<double> revenue(n_csp, 0.00);
	for(int iter=1;iter<=n_iterations;iter++){
		//cout << "Iteration : "<<iter<<endl;
		// NOTE: use user_rating of iter-1'th index
		if(iter%freq==0){
			// Fetch Points {iter: [revenues for csps]} 
			// cout <<endl<<endl<<"I :"<<iter<<endl;

			// Apply dynamic pricing
			for(int u=0;u<n_users; u++){
				//cout<<" User: "<<u<<" -----------"<<endl;
				for(int c=0;c<n_csp;c++){
					for(int res=0;res<n_resource;res++){
						updateData(u,c,res, getDynamicPrice(c,res,u,csps,csp_manager, iter), csps, csp_manager);
					}
					//csps[c].printData(c);
				}
			}
			//csp_manager.printData();

			// cout<<" Purchase History"<<endl;
			// for(int i=0;i<n_csp;i++){
			// 	cout<<" "<<csp_manager.getAcceptedResource(i);
			// }
			// cout<<endl;

			//cout<<find_jain(revenue)<<endl;
			cout<<" Current revenue:"<<endl;	
			for(int c=0;c<n_csp;c++){
				cout<<revenue[c]<<" ";
				//revenue[c] = 0;
			}
			cout<<endl;
		}
		// Using previous Job Ratings to update things for users
		updateLocalTrust(iter);
		for(int u=0;u<n_users;u++){
			// reset user's utility
			for(int res=0;res<n_resource;res++){
				users[u].util_res[res].second = -1;
				users[u].util_res[res].first = INT_MIN;
			}
			// for every csp
			for( int c=0; c<n_csp; c++){
				updateReferenceCredit(u,c,iter);
				users[u].ref_trust[c] = getReferenceTrust(u,c,iter);
				for(int res=0; res<n_resource; res++){
					double utility = computeUtility(u,c,res,iter);
					//cout<<"U "<<utility<<endl;
					if(utility> users[u].util_res[res].first){
						// u-user finds resouce-res by csp c best!
						users[u].util_res[res].second = c;
						users[u].util_res[res].first = utility;
					}
				}
			}
			// User decided, which csp to work with.
			// users[u].util_res[res] contains the chosen csp index.
			for(int res=0;res<n_resource;res++){
				int chosen_csp = users[u].util_res[res].second;
				csp_manager.setAcceptedResource(chosen_csp);
				revenue[chosen_csp] += csps[chosen_csp].getPrice(u, res);
			}

			// Now Update new Job Ratings
			updateJobRatings(u, iter);

		}
	}
}



int main(){
	//srand (time(NULL));
	for(int i=0; i<n_csp; i++){
		CSP csp;
		csps.push_back(csp);
		csps[i].printData(i);
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
	return 0;
}
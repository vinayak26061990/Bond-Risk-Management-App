#include <stdlib.h>
#include "Hedging.h"
using namespace std;

Hedging::~Hedging()
{
map<string, vector<Instrument *>*>::iterator it;
for (it = bucketTicker.begin(); it != bucketTicker.end(); it++) 
{
delete it->second;
}	

}

Hedging::Hedging(SBB_instrument_fields records[],Instrument*  records_calc[],int length)
 {
	
	static SBB_date settle_dt_obj, mat_dt_obj;
	r = records;
	rc = records_calc;
	count=length;
	ticker_size = 0;

	for(int i=0;i<count;i++)
	 {
        settle_dt_obj.set_from_yyyymmdd(records[i].SettlementDate());
		mat_dt_obj.set_from_yyyymmdd(records[i].MaturityDate());
		int T= SBB_term::get_num_periods(settle_dt_obj, mat_dt_obj,1);
        //printf("T is %d\n",T);
		if (T>0 && T<=2)
			bucket0_2.push_back(records_calc[i]);
		else if(T>2 && T<=5)
			bucket2_5.push_back(records_calc[i]);
		else if(T>5 && T<=10)
			bucket5_10.push_back(records_calc[i]);
		else if(T>10 && T<=30)
			bucket10_30.push_back(records_calc[i]);

	
		
		
		if (strcmp(records[i].Quality(),"AAA")==0) {
			bucketAAA.push_back(records_calc[i]);
		
		}
		else if (strcmp(records[i].Quality(),"AA")==0) {
			bucketAA.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"A")==0) {
			bucketA.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"BBB")==0) {
			bucketBBB.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"BB")==0) {
			bucketBB.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"B")==0) {
			bucketB.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"CCC")==0) {
			bucketCCC.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"CC")==0) {
			bucketCC.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"C")==0) {
			bucketC.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality(),"D")==0) {
			bucketD.push_back(records_calc[i]);
		}

		map<string, vector<Instrument *>*>::iterator it;
		it = bucketTicker.find(records[i].Ticker());
		if(it != bucketTicker.end()) {
			it->second->push_back(records_calc[i]);
		}
		else {
			vector<Instrument*> *vec = new vector<Instrument *>();
			vec->push_back(records_calc[i]);
			bucketTicker.insert(make_pair(records[i].Ticker(), vec)); 
			ticker_size++;
		}


	
	}
	
	
	//printf ("size of bucket 2 %d\n",bucket0_2.size());
	//printf ("size of bucket 5 %d\n",bucket2_5.size());
	//printf ("size of bucket 10 %d\n",bucket5_10.size());
	//printf ("size of bucket 30 %d\n",bucket10_30.size());


}



double Hedging::cal_market_value(Instrument** records_calc,int count, int shift)
{
	double total_market_value = 0;
	for (int i=0; i< count; i++)
	{      double mktval=records_calc[i]->getshiftmktval(shift,records_calc[i]);
		total_market_value += mktval;
	}
	return total_market_value;
}

double Hedging::cal_2yr_hedge_amount(vector<Instrument*> bucket,double &risk,double dv01, double shift)   
{
	double bucket_risk=0;
	for (unsigned int i=0; i<bucket.size(); i++)
	{      
		bucket_risk +=bucket[i]->shiftrisk(shift);
	}
	risk=bucket_risk;
	return bucket_risk/dv01;
}

double Hedging::cal_bucket_mktval(vector<Instrument*> bucket,double shift)
{
	double bucket_mktval = 0;
	//printf("The Bucket size is %d\n",bucket.size());
	for (unsigned int i=0; i<bucket.size();i++)
	{
		bucket_mktval += bucket[i]->getshiftmktval(shift,bucket[i]);	
	}
	//printf("The bucket_mktval is %.3f\n",bucket_mktval);
	return bucket_mktval;
}







double Hedging::cal_amount(double dvo1)
{
	double bucket_risk=0;
	for (unsigned int i=0; i<bucket10_30.size(); i++)
	{      
		bucket_risk +=bucket10_30[i]->get_bond_risk();
	}
	return bucket_risk/dvo1;
}


double Hedging::getTotalLGD()
{
	double total_LGD = 0;
	for(int i=0; i<count; i++) {
		total_LGD += rc[i]->get_bond_lgd();
	}
	return total_LGD;
}

int Hedging::getTotalAmount()
{
	int total_amount=0;
	for (int i=0; i<count; i++)
	{
		total_amount += r[i].Amount();
	}
	return total_amount;
}




int Hedging::getbucketAmount(vector<Instrument*>* bucket)
{

	int amt = 0;
	//printf("Inside");
	for(unsigned int i=0;i<bucket->size();i++) {
		amt += (*bucket)[i]->getAmount();
	}
	return amt;

}

double Hedging::getbucketrisk(vector<Instrument*>* bucket,double shift)
{

	double risk = 0;
	for(unsigned int i=0;i<bucket->size();i++) {
		risk += (*bucket)[i]->shiftrisk(shift);
	}
	return risk;

}
double Hedging::getbucketLGD(vector<Instrument*>* bucket)
{

	double bucketlgd = 0;
	for(unsigned int i=0;i<bucket->size();i++) {
		bucketlgd += (*bucket)[i]->get_bond_lgd();
	}
	return bucketlgd;

}


output* Hedging::getPositions(double shift)
{
	output* p_array = new output[ticker_size]();
    
	map<string,vector<Instrument*>*>::iterator it=bucketTicker.begin();
	for(int i=0;i<ticker_size;i++) {
		vector<Instrument*>* bucket = it->second;
		output *p = &p_array[i];
		p->Ticker = it->first;
		//printf("inside getpos\n");
		it++;
		p->Amount = getbucketAmount(bucket);
		p->Risk = getbucketrisk(bucket,shift);
		p->LGD = getbucketLGD(bucket);
	}
	return p_array;
}

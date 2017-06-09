#ifndef HEDGING_H
#define HEDGING_H
#include "Instrument.h"
#include <vector>
#include <map>
using namespace std;

struct output {
	string Ticker;
	int Amount;
	double Risk;
	double LGD;
};



class Hedging  {

	public:
		~Hedging();
		output* getPositions(double shift);
		Hedging( SBB_instrument_fields records[],Instrument*  records_calc[],int length);
        double cal_amount(double dvo1);
		double getTotalLGD();
		int getTotalAmount();
		double cal_market_value(Instrument** records_calc,int item_count, int shift);
		double cal_2yr_hedge_amount(vector<Instrument*> bucket,double &risk,double dv01, double shift);
		double cal_bucket_mktval(vector<Instrument*> bucket, double shift);
		static char* dailychangebyissuer(Hedging* opening_port,Hedging* closing_port,double shift)
		{
			char* buffer = new char[1024];
			
			char* b = new char[2048];
			b[0]='\0';
			 int j = 0;
		
			
			int n,openamt,closeamt;
			output* openout = opening_port->getPositions(shift);
			output* closeout = closing_port->getPositions(shift);
   			double closerisk=0,openrisk=0,closelgd=0,openlgd=0;
			//printf("Opening port ticker size %d\n",opening_port->ticker_size);
			//for(int i=0;i<opening_port->ticker_size;i++) {
			for(int i=0;i<opening_port->ticker_size;i++) {
				//printf("Initialized for i %d\n",i);
				openamt = openout[i].Amount;
				//printf("Initialized for open amount %d\n",openamt);
				closeamt = closeout[i].Amount;
     			//printf("Initialized for close amount %d\n",closeamt);
				openrisk = openout[i].Risk;
       			//printf("Initialized for open risk %.3f\n",openrisk);
				closerisk = closeout[i].Risk;
       			//printf("Initialized for close risk %.3f\n",closerisk);
				openlgd = openout[i].LGD;
       			//printf("Initialized for open lgd %.3f\n",openlgd);
				closelgd = closeout[i].LGD;
       			//printf("Initialized for close lgd %.3f\n",openlgd);
				//printf("Ticker %s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;\n",openout[i].Ticker.c_str(),openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
				n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;",openout[i].Ticker.c_str(),openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
       			//printf("Added to buffer\n");
				strcat(b,buffer);
       			//printf("Concatenated to buffer\n");

			}
			// AAA
			openamt = opening_port->getbucketAmount(&opening_port->bucketAAA);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketAAA);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketAAA,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketAAA,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketAAA);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketAAA);
			
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","AAA",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_ic+=openamt;
			//n_ie+=closeamt;
			//r_ic+=openrisk;
			//r_ie+=closerisk;
			//l_ic+=openlgd;
			//l_ie+=closelgd;
			
			// AA
			openamt = opening_port->getbucketAmount(&opening_port->bucketAA);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketAA);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketAA,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketAA,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketAA);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketAA);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","AA",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_ic+=openamt;
			//n_ie+=closeamt;
			//r_ic+=openrisk;
			//r_ie+=closerisk;
			//l_ic+=openlgd;
			//l_ie+=closelgd;
			
			// A
			openamt = opening_port->getbucketAmount(&opening_port->bucketA);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketA);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketA,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketA,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketA);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketA);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","A",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_ic+=openamt;
			//n_ie+=closeamt;
			//r_ic+=openrisk;
			//r_ie+=closerisk;
			//l_ic+=openlgd;
			//l_ie+=closelgd;
			
			// BBB
			openamt = opening_port->getbucketAmount(&opening_port->bucketBBB);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketBBB);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketBBB,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketBBB,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketBBB);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketBBB);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","BBB",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_ic+=openamt;
			//n_ie+=closeamt;
			//r_ic+=openrisk;
			//r_ie+=closerisk;
			//l_ic+=openlgd;
			//l_ie+=closelgd;
			
			// BB
			openamt = opening_port->getbucketAmount(&opening_port->bucketBB);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketBB);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketBB,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketBB,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketBB);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketBB);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","BB",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_jc+=openamt;
			//n_je+=closeamt;
			//r_jc+=openrisk;
			//r_je+=closerisk;
			//l_jc+=openlgd;
			//l_je+=closelgd;
			
			// B
			openamt = opening_port->getbucketAmount(&opening_port->bucketB);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketB);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketB,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketB,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketB);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketB);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","B",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_jc+=openamt;
			//n_je+=closeamt;
			//r_jc+=openrisk;
			//r_je+=closerisk;
			//l_jc+=openlgd;
			//l_je+=closelgd;
			
			// CCC
			openamt = opening_port->getbucketAmount(&opening_port->bucketCCC);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketCCC);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketCCC,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketCCC,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketCCC);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketCCC);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","CCC",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_jc+=openamt;
			//n_je+=closeamt;
			//r_jc+=openrisk;
			//r_je+=closerisk;
			//l_jc+=openlgd;
			//l_je+=closelgd;
			
			// CC
			openamt = opening_port->getbucketAmount(&opening_port->bucketCC);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketCC);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketCC,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketCC,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketCC);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketCC);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","CC",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_jc+=openamt;
			//n_je+=closeamt;
			//r_jc+=openrisk;
			//r_je+=closerisk;
			//l_jc+=openlgd;
			//l_je+=closelgd;
			
			// C
			openamt = opening_port->getbucketAmount(&opening_port->bucketC);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketC);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketC,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketC,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketC);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketC);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","C",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_jc+=openamt;
			//n_je+=closeamt;
			//r_jc+=openrisk;
			//r_je+=closerisk;
			//l_jc+=openlgd;
			//l_je+=closelgd;
			
			// D
			openamt = opening_port->getbucketAmount(&opening_port->bucketD);
			closeamt = closing_port->getbucketAmount(&closing_port->bucketD);
			openrisk = opening_port->getbucketrisk(&opening_port->bucketD,shift);
			closerisk = closing_port->getbucketrisk(&closing_port->bucketD,shift);
			openlgd = opening_port->getbucketLGD(&opening_port->bucketD);
			closelgd = closing_port->getbucketLGD(&closing_port->bucketD);
			n = sprintf(buffer,"%s,%d,%.3f,%.3f,%d,%.3f,%.3f,%d,%.3f,%.3f;","D",openamt,openrisk,openlgd,closeamt,closerisk,closelgd,closeamt-openamt,closerisk-openrisk,closelgd-openlgd);
			strcat(b,buffer);
			//n_jc+=openamt;
			//n_je+=closeamt;
			//r_jc+=openrisk;
			//r_je+=closerisk;
			//l_jc+=openlgd;
			//l_je+=closelgd;
			
			// Investment
			//n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","Investment_Grade",n_ic,r_ic,l_ic,n_ie,r_ie,l_ie,n_ie-n_ic,r_ie-r_ic,l_ie-l_ic);
			//strcat(b,buffer);
			// Junk
			//n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","Junk",n_jc,r_jc,l_jc,n_je,r_je,l_je,n_je-n_jc,r_je-r_jc,l_je-l_jc);
			//strcat(b,buffer);

			delete [] openout;
			delete [] closeout;
			return b;







		}
		int getbucketAmount(vector<Instrument*>* bucket);
		double getbucketrisk(vector<Instrument*>* bucket,double shift);
		double getbucketLGD(vector<Instrument*>* bucket);
		
		vector<Instrument*> bucket0_2;
		vector<Instrument*> bucket2_5;
		vector<Instrument*> bucket5_10;
		vector<Instrument*> bucket10_30;
		vector<Instrument*> bucketAAA;
		vector<Instrument*> bucketAA;
		vector<Instrument*> bucketA;
		vector<Instrument*> bucketBBB;
		vector<Instrument*> bucketBB;
		vector<Instrument*> bucketB;
		vector<Instrument*> bucketCCC;
		vector<Instrument*> bucketCC;
		vector<Instrument*> bucketC;
		vector<Instrument*> bucketD;

		int ticker_size;
		map<string, vector<Instrument*>*> bucketTicker;
		SBB_instrument_fields* r;
		Instrument**  rc;
		int count;
};

#endif

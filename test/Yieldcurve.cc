#include "Yieldcurve.h"
#include <math.h>
#include  <stdlib.h>
#include "Instrument.h"

Yieldcurve::Yieldcurve(SBB_instrument_fields* input,int totalrecord)
{
param=input;
records=totalrecord;

Yield_period=new int[records];
for(int i=0;i<records;i++)
{
SBB_instrument_fields* bond_record_ptr = &param[i];
static SBB_date settle_dt_obj, mat_dt_obj;
settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate());
mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate());
Yield_period[i]= SBB_term::get_num_periods(settle_dt_obj, mat_dt_obj,bond_record_ptr->Frequency());

}
/*
printf("Yield array is\n");
for(int i=0;i<records;i++)
{
printf(" %d",Yield_period[i]);
}
printf("\n");
*/
}

/*
double Yieldcurve::get_yield(int periods)
{

//printf("\n");
//printf("period from tradingbook inside yieldcurve is %d\n",periods);

int min=abs(periods-Yield_period[0]);
int dif;
int i=0;int j=1;
while( j < records )
{
dif=abs(periods-Yield_period[j]);
if(dif < min)
{
min=dif;
i=j;
}
j++;
}

//printf("i and min inside get_yield is %d,%d \n",i,min);

//shiftyield=param[i].Yield() + shift_arr[i];
return param[i].Yield();
}
*/

double Yieldcurve::getdvo1(int term){
	char sid[32];
	sprintf(sid,"T%d",term);

	for (int i=0; i<records; i++)
	{
		if (0==strcmp(param[i].SecurityID(),sid))
		{
			SBB_instrument_fields* bond_record_ptr= &param[i];
			CouponBond cb(bond_record_ptr,Yield_period[i]);
			
			return cb.get_bond_DV01();
		}
	}
}

//double Yieldcurve::getshiftyield(int term)
//{
//double res=get_yield(term);
//printf("Shiftyield is \n %.2f",shiftyield);
//return shiftyield;
//}



void Yieldcurve::set_arr(double s1, double s2, double s3, double s4)
{
	for(int i=0;i<records;i++) 
	{
		const char *sec=param[i].SecurityID();
		printf("Security ID is %s \n",sec);
		char x=sec[1];
		printf("Pattern is %c \n",x);

		if(x=='2')
			
			shift_arr[i]=s1;
		else if(x=='5')
			shift_arr[i]=s2;
		else if(x=='1')
			shift_arr[i]=s3;
		else if(x=='3')
		
			shift_arr[i]=s4;
			
	}
	

}



void Yieldcurve::getyieldid(int term, int& idt, double& yield)
{
static SBB_date settle_dt_obj, mat_dt_obj;
        int termd=0, termu=0, id=-1, iu=-1;

        for(int i=0;i<records;i++) {
			SBB_instrument_fields* bond_record_ptr = &param[i];
			
			settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate());
			mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate());
            int t = term - SBB_term::get_num_periods(settle_dt_obj, mat_dt_obj, 1);

                if(t<0)
                	if(-1==iu) {
                        	iu = i;
                                termu = t;
                        }
                        else if(t<termu) {
                                iu = i;
                                termu = t;
                        }
                else {
                        if(-1==id) {
                                id = i;
                                termd = t;
                        }
                        else if(t>termd) {
                                id = i;
                                termd = t;
                        }
                }
        }

        if(id==-1){
				//printf("The yield is param[id].Yield() %.3f\n",param[iu].Yield());
				//printf("The increment value is %.3f\n",shift_arr[iu]);
                yield = param[iu].Yield()+shift_arr[iu];
				idt = iu;
		}
        else{
				
                yield = param[id].Yield()+shift_arr[id];
				idt = id;
		}
}

double Yieldcurve::getyield(int term) {
	int id;
	double yield;
	getyieldid(term,id,yield);
	return yield;
}

int Yieldcurve::getID(int term) {
	int id;
	double yield;
	getyieldid(term,id,yield);
	return id;
}

double Yieldcurve::getyielduseid(int id)
{
	//printf("param_id yield is %.3f\n",param[id].Yield());
	//printf("Shift array value is %.6f\n",shift_arr[id]);
	//printf("Get Yield use id is %.3f\n",param[id].Yield()+shift_arr[id]);
	//printf("Shift array now is\n");
	//for (int i=0;i<4;i++)
	//{
	//	printf("%.3f ",shift_arr[i]);
	//}
	//printf("\n");
	return param[id].Yield()+shift_arr[id];

}





























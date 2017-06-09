#include <unistd.h>
#include <sys/times.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "Instrument.h"
#include <iomanip>
#include <stdlib.h>
#include <math.h>


struct output
{
	const char *input;
	double price;
	double DV01;
	double risk;
	double lgd;
};

int main(int argc, char *argv[])
{      
       if(argc != 3)
	{
		printf("testdriver input yieldcurve\n");
		exit(0);
	}
	SBB_util obj;
	obj.start_clock();
       
       SBB_instrument_input_file input(argv[1]);
       SBB_instrument_input_file yieldcurve(argv[2]);
       int length=0;
       SBB_instrument_fields* yieldrecords=yieldcurve.records(length);
       Yieldcurve y(yieldrecords,length);              
       SBB_instrument_fields* records=input.records(length); 
       output* res = new output[length];
       int total_amount=0;
       int Amount;
       double total_risk=0.0;
       double risk=0.0; 
	double price=0; 
	double DV01=0; 
	double lgd=0;
       int largest_long= 0, largest_short = 0;
	double largest_risk = 0.0;






       for(int i=0;i<length;i++)
       {


			SBB_instrument_fields* bond_record_ptr = &records[i];
                     static SBB_date settle_dt_obj, mat_dt_obj;
                     settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate());
			mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate());
			int num_periods = SBB_term::get_num_periods(settle_dt_obj, mat_dt_obj,bond_record_ptr->Frequency());
			Instrument* bond_calc_ptr;
                     
			if(0 == bond_record_ptr->Coupon()) 
			{
                      
			 bond_calc_ptr = new ZeroCouponBond(bond_record_ptr,num_periods,&y);                      
			}
			else 	// must be coupon bearing - only two types in the file
			{
			 bond_calc_ptr = new CouponBond(bond_record_ptr,num_periods,&y);
			}
                     res[i].input=bond_record_ptr->Line_buffer();
                     res[i].price=bond_calc_ptr->get_bond_price();
 			res[i].DV01=bond_calc_ptr->get_bond_DV01();
                     risk=bond_calc_ptr->get_bond_risk();
              	res[i].risk=risk;
              	res[i].lgd=bond_calc_ptr->get_bond_lgd();
                    
            
        		if(abs(risk) > abs(largest_risk))
			{
			largest_risk = risk;
			}
			total_risk += risk;
			Amount = bond_record_ptr->Amount();
			
			if(Amount < largest_short)
                     {
			largest_short = Amount;
		       }
			
			else if( Amount > largest_long)
			{
			largest_long = Amount;
			}
              	//printf("\n");
	}
       

      	FILE *result = fopen("tradingbook_results.txt", "w");
       if (!result)
	{
		fprintf(stderr,"ERROR %s couldn't open: tradingbook_results.txt\n", __PRETTY_FUNCTION__);
		printf("file open failure\n");
	}

       for(int i=0;i<length;i++)
       {
          fprintf(result, "%s %.2f %.3f %.3f %.3f\n",res[i].input,res[i].price, res[i].DV01, res[i].risk, res[i].lgd);

       }
       fclose(result);

       
       FILE *f = fopen("results.txt", "w");
	if (!f){
		fprintf(stderr,"ERROR %s couldn't open: results.txt\n", __PRETTY_FUNCTION__);
		printf("file open failure\n");
	}

	fprintf(f, "%d\n", largest_long);
	fprintf(f, "%d\n", largest_short);
	fprintf(f, "%.3f\n", largest_risk);
	fprintf(f, "%.3f\n", total_risk);
	input.free_records();
       yieldcurve.free_records();
       fclose(f);
       obj.end_clock();
       return 0;
}
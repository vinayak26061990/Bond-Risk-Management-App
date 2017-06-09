#include "Instrument.h"
#include <math.h>

double Instrument::get_bond_DV01(double shift)
{
	double low_price, high_price;
	low_price=get_bond_price(getrate()+shift-0.01);
	high_price=get_bond_price(getrate()+shift+0.01);
    dv01=(low_price - high_price)/2;
	return dv01;

}

Instrument::Instrument(SBB_instrument_fields *input,int period,Yieldcurve *c)
{
	param = input;
	periods = period;
    y=c;
       
    if(param->is_spread_priced())
      {
           //double bp=y->get_yield(periods);
           //param->ValType("YIELD");
           //param->Yield(bp+param->Spread()/100);
		   id=y->getID(periods/param->Frequency());
		   //printf("id for spread is %d\n",id);
		   //printf("Spread val is %.3f",param->Spread());
		   //yield = (y->getYieldUseID(id)) + param->Spread()/100;
		   
   		   printf("The final yield is %.3f\n",getrate());


      }
	//yield=param->Yield();
	else
	{id=-1;}
	lgd = ob.LGD_given_SnP_Fitch(param->Quality()) * param->Amount();

      
}



double Instrument::get_bond_lgd()
{
  return lgd;
} 


double Instrument::get_bond_risk(double y)
{

  return (get_bond_DV01(y)*param->Amount())/100;

}

double Instrument::shiftrisk(double shift) 
{
	
	return (get_bond_DV01(shift)*param->Amount())/100;
}

double Instrument::getshiftmktval(double shift,Instrument* par) 
{
	double shifted_mkt_val=0;
	//printf("Amount is %d\n",param->Amount());
	//printf("Security Id is %s\n",param->SecurityID());
	//printf("Shift is %.3f\n",shift);
	//printf("Yield or spread is %.3f\n",par->getrate());
	//printf("Bond price is %.6f\n",par->get_bond_price(par->getrate()+shift));
	shifted_mkt_val=(param->Amount()*par->get_bond_price(par->getrate()+shift))/100;
	//printf("shifted market val is %.3f\n",shifted_mkt_val);
	return shifted_mkt_val;
}
double CouponBond::get_bond_price(double y)
{      
	double ratio=((y/100)/param->Frequency());
	double factor = 1/pow(1 + ratio, periods);
	double coupon_term = ((param->Coupon()/param->Frequency()) * (1- factor) /ratio);
	double principle_term = 100*factor;
    price=coupon_term + principle_term;
	return price;
}

double ZeroCouponBond::get_bond_price(double y)
{     
    price=100 * pow(1+y/100/param->Frequency(), -periods);
	return price;

}


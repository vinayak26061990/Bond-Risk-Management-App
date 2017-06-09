#ifndef __Instrument_H__
#define __Instrument_H__
#include "Yieldcurve.h"
#include "SBB_ratings.h"


class Instrument
{
	public:
		Instrument(SBB_instrument_fields* input,int period,Yieldcurve *c=NULL);
		virtual double get_bond_price(double shift){};
		double get_bond_DV01(double shift=0);
        double get_bond_risk(double shift=0);
        double get_bond_lgd();
        double get_sum_bond_lgd();
		double getshiftmktval(double shift,Instrument*);
		double shiftrisk(double shift=0);
		double getrate() { 	double yield;
							//printf("id is \n %d",id);
								if(id==-1)
									yield = param->Yield();
							else {
								   //printf("Spread val is %.3f\n",param->Spread());
								   //printf("y->getYieldUseID(id) %.3f\n",y->getyielduseid(id));
								   yield = y->getyielduseid(id) + param->Spread()/100;
								   
								 } return yield;}
		void   setyield(double y) {param->Yield(y);}
		int    getAmount() {return param->Amount();}


	protected:
              int id;
			  Yieldcurve *y;
              SBB_instrument_fields* param;
              int periods;
              double dv01,price,lgd;
              SBB_bond_ratings ob;
			  //double yield;
           

};


class ZeroCouponBond:public Instrument 
{
    public:
    ZeroCouponBond(SBB_instrument_fields* param,int period,Yieldcurve *c=NULL):Instrument(param,period,c){}
    double get_bond_price(double y);
};

class CouponBond:public Instrument
{
    public:
    CouponBond(SBB_instrument_fields* param,int period,Yieldcurve *c=NULL):Instrument(param,period,c){}
    double get_bond_price(double y);
   
};    

#endif

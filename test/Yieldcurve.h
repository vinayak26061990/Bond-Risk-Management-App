#ifndef __Yieldcurve_H__
#define __Yieldcurve_H__
#include "SBB_io.h"
#include "SBB_date.h"


class Yieldcurve
{
public:
Yieldcurve(SBB_instrument_fields* input,int totalrecord);
//double get_yield(int periods);
void set_arr(double s1, double s2, double s3, double s4);
//double getshiftyield(int term);
SBB_instrument_fields* get_yielddata(int i)
{
  return param+i;
};
double getdvo1(int term);
void getyieldid(int term, int& idt, double& yield);
double getyield(int term);
int getID(int term);
double getyielduseid(int id);







private:
SBB_instrument_fields* param;
int *Yield_period;int records;
double shift_arr[4];
double yield;













};

#endif

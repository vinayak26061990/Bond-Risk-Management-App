#ifndef VAR_H
#define VAR_H
#include "Instrument.h"
#include <vector>
#include<stdlib.h>
class Var
{


public:
~Var();
Var(char *c,SBB_instrument_fields* r,Instrument** instr,int len);
void calculatevar(double& valueatrisk,double& valueatrisk_cs,double& valueatrisk_ir,double** PnL, int& size, double shift);
static char* dailychangebyissuer(Var* vo, Var* vc, double shift)
	{
		char buffer[16384];
		char*p = new char[16384];
		memset(p,0,sizeof(p));
		double o,c,ocs,oir,ccs,cir;
		double *po;
		double *pc;
		int count;
		//printf("Inside\n");
		vo->calculatevar(o,ocs,oir,&po,count,shift);
		vc->calculatevar(c,ccs,cir,&pc,count,shift);
		printf("count is %d\n",count);

		//printf("VaR - credit(spread) opening %.3f closing %.3f\n",ocs,ccs);
		sprintf(buffer,"VaR - credit,%.3f,%.3f,%.3f;",ocs,ccs,ccs-ocs);
		strcat(p,buffer);
		//printf("VaR - rate(yield) opening %.3f closing %.3f\n",oir,cir);
		sprintf(buffer,"VaR - rate,%.3f,%.3f,%.3f;",oir,cir,cir-oir);
		strcat(p,buffer);
		sprintf(buffer,"Var Total,%.3f,%.3f,%.3f;",o,c,c-o);
		//printf("VaR - total opening %.3f closing %.3f\n",o,c);
		strcat(p,buffer);
		//count=100;
		for(int i=0;i<count;i++) 
		{
			//sprintf(buffer,"%.3f,",pc[i]/100);
			sprintf(buffer,"%.3f,",pc[i]);
			strcat(p,buffer);
		}
		//printf("Data is written \n");
		//memset(&buffer[0], 0, sizeof(buffer));
		delete [] po;
		delete [] pc;
		return p;
	}

private:
char *dir;
SBB_instrument_fields* fields;
Instrument** calc;
int length;
SBB_var_day_record* arr2;
SBB_var_day_record* arr5;
SBB_var_day_record* arr10;
SBB_var_day_record* arr30;





};


#endif

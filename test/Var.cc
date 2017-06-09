#include "Var.h"
#include <string.h>
Var::~Var()
{
delete [] arr2;
delete [] arr5;
delete [] arr10;
delete [] arr30;
}

Var::Var(char *c,SBB_instrument_fields* r,Instrument** instr,int len)
{

length=len; 
dir=c;
fields=r; 
calc=instr;

int size;
char buffer[128];
sprintf(buffer, "%s%s",dir,"T2.txt");
SBB_var_input_file var2;
var2.open(buffer);
arr2 = var2.records(size);
sprintf(buffer, "%s%s",dir,"T5.txt");
SBB_var_input_file var5;
var5.open(buffer);
arr5 = var5.records(size);
sprintf(buffer, "%s%s",dir,"T10.txt");
SBB_var_input_file var10;
var10.open(buffer);
arr10 = var10.records(size);
sprintf(buffer, "%s%s",dir,"T30.txt");
SBB_var_input_file var30;
var30.open(buffer);
arr30 = var30.records(size);
}

void Var::calculatevar(double& valueatrisk,double& valueatrisk_cs,double& valueatrisk_ir,double** PNL, int& count, double shift)
{
    *PNL = new double[1000]();
	double *PNL_CS = new double[1000]();
	double *PNL_IR = new double[1000]();
	int size;

	for(int i=0;i<length;i++)
	 {
		double rate_array[1001];
		double rate_CS[1001];
		double rate_IR[1001];
		//double initial_rate=calc[i]->getrate();
		double rate = calc[i]->getrate()+shift;
		//printf("rate is %.3f\n",rate);
		//calc[i]->setyield(rate);
		double price = calc[i]->get_bond_price(rate);
		//printf("price is %.3f\n",price);
		//calc[i]->setyield(initial_rate);
		char buffer[128];
		//printf("i is %d\n",i);
		//printf("dir is %s\n",dir);
		//fields[i].show();
		//printf("Show done\n");
		const char *abc=fields[i].SecurityID();
		
		//printf("name is %s\n",abc);
		//printf("Done");
		char dirtemp[10],temp[10];
		//printf("name char is %s\n",abc);
		//sprintf(buffer,"%s",dir);
		//printf("buffer is %s",buffer);
		strcpy(dirtemp,dir);
	    //strcat(buffer,temp);
		strcpy(temp,abc);
	    //strcat(buffer,temp);
		//printf("buffer is %s\n",buffer);
	    //strcat(buffer,".txt");
		sprintf(buffer,"%s%s%s",dirtemp,temp,".txt");
		//printf("buffer is %s\n",buffer);
		SBB_var_input_file sbb1;
		//const char* name=buffer;
		//sprintf(buffer,"%s",name);
		sbb1.open(buffer);
		
		SBB_var_day_record* varrecords = sbb1.records(size);
		//printf("size is %d\n",size);
		//int flag=0;
		//flag=varrecords[0].is_yield_priced();
		//printf("abc is %d\n",flag);
		
		if(varrecords[0].is_yield_priced()) 
		{   
	
			//printf("Yeah yield\n");
			for(int j=0;j<size;j++)
			{
				rate_array[j] = varrecords[j].ValValue();
				rate_CS[j] = 0.0;
				rate_IR[j] = varrecords[j].ValValue();
     			//printf("value in rate_IR is %.2f\n",rate_IR[j]);

			}
		    //printf("Its yield\n");

		}
		else if(varrecords[0].is_spread_priced())
	      	      {
		    //printf("Its spread\n");
			SBB_var_day_record* bm;
			//varrecords[0].show();
			char *x=varrecords[0].BenchmarkID();
			//printf("x is %s",x);
			/*
			switch(x[1]) 
			{
			case '2':
				bm = arr2; 
				break;
			case '5':
				bm = arr5; 
				break;
			case '1':
				bm = arr10; 
				break;
			case '3':
				bm = arr30; 
				break;
			default:
				break;
			}*/
			
			if(x[1]=='2')
				 {
			         bm = arr2; 
              		  //printf("The char is %c \n",x[1]);
				 }
			else if(x[1]== '5')
				{
				 bm = arr5; 
				 //printf("The char is %c \n",x[1]);

				}
			else if (x[1]=='1')
				{
				 bm = arr10; 
				//printf("The char is %c \n",x[1]);

			       }
			else if (x[1]== '3')
				{
				 bm = arr30;
				 //printf("The char is %c \n",x[1]);

				} 
			else
				{	
				  exit(0);
				}

			
			for(int j=0;j<size;j++) 
			{
				rate_array[j] =bm[j].ValValue() + (varrecords[j].ValValue())/100 ;
				rate_CS[j] = varrecords[j].ValValue() / 100;
				rate_IR[j] = bm[j].ValValue();

			}
		}
		double y=0;
		for(int j=1;j<size;j++) 
		{
			double r = rate + rate_array[j] - rate_array[j-1];
			double rc= rate + rate_CS[j] - rate_CS[j-1];
			double ri= rate + rate_IR[j] - rate_IR[j-1];
			y=calc[i]->get_bond_price(r);
			(*PNL)[j-1] = (*PNL)[j-1] + (y-price) * fields[i].Amount()/100;
			y=calc[i]->get_bond_price(rc);
			PNL_CS[j-1]= PNL_CS[j-1] + (y-price)*fields[i].Amount();
			y=calc[i]->get_bond_price(ri);
            PNL_IR[j-1]= PNL_IR[j-1] + (y-price)*fields[i].Amount();
		}
		//printf("\n");
		
	}
	
	double output = (*PNL)[0];
	double output_CS = PNL_CS[0];
	double output_IR = PNL_IR[0];
	for(int k=1;k<size-1;k++) 
	{
		if(output >= (*PNL)[k])
		   {
			output =(*PNL)[k];
		   }
		if (output_CS >=PNL_CS[k]) 
		   {
		    output_CS =PNL_CS[k];
		   }
              if(output_IR >= PNL_IR[k])
		   { 
		    output_IR=PNL_IR[k];
		   }

		

	}
count=size-1;
printf("Count of pnl is %d",count);
valueatrisk = output;
valueatrisk_cs = output_CS/100;
valueatrisk_ir = output_IR/100;
delete [] PNL_CS;
delete [] PNL_IR;
//printf("Calculation done \n");
}

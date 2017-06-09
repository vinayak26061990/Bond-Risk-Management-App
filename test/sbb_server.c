#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "sbb_socket.h"
#include <time.h>   
#include <stdint.h>
#include "Instrument.h"
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include "Hedging.h"
#include "Var.h"
#include <Windows.h>
double get_cpu_time(){
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }else{
        //  Handle error
        return 0;
    }
}

struct midoutput
{
	const char *input;
	double price;
	double DV01;
	double risk;
	double lgd;
};

char * gui_result(char * msg, Yieldcurve &yc, Hedging &open_hedge, Hedging &close_hedge, Var &Openvar, Var &Closevar)
{
	double shift,spread1,spread2,spread3,spread4;
	char * token;
	token = strtok(msg,",");
	if(NULL == token){
		fprintf(stderr,"cannot read client msg: %s\n",msg);
		exit(1);
	}
	shift = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot read client msg: %s\n",msg);
		exit(1);
	}	
	spread1 = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot read client msg: %s\n",msg);
		exit(1);
	}		
	spread2 = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot read client msg: %s\n",msg);
		exit(1);
	}
	spread3 = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot read client msg: %s\n",msg);
		exit(1);
	}
	spread4 = atof(token);
	
	//initialize yc to 0, get original statistics
	yc.set_arr(0,0,0,0);

	double dv01_2yr = yc.getdvo1(2);
    double yc2,yc5,yc10,yc30;
    printf("dv01 before shift is %.3f \n",dv01_2yr);
    printf("shift is %.3f \n",shift);

	yc2 = yc.getyield(1);
	yc5 = yc.getyield(4);
	yc10 = yc.getyield(9);
	yc30 = yc.getyield(29);
	printf("yc2 %.3f yc5 %.3f yc10 %.3f yc30 %.3f\n",yc2,yc5,yc10,yc30);
	//printf("Before shift\n");
	//before shift
    //printf("dv01 before shift is %.3f \n",dv01_2yr);

	double R2,M2,H2,R5,M5,H5,R10,M10,H10,R30,M30,H30;
	H2 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket0_2,R2,dv01_2yr,0);
	M2 = close_hedge.cal_bucket_mktval(close_hedge.bucket0_2,0);
	H5 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket2_5,R5,dv01_2yr,0);
	M5 = close_hedge.cal_bucket_mktval(close_hedge.bucket2_5,0);
	H10 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket5_10,R10,dv01_2yr,0);
	M10 = close_hedge.cal_bucket_mktval(close_hedge.bucket5_10,0);
	H30 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket10_30,R30,dv01_2yr,0);
	M30 = close_hedge.cal_bucket_mktval(close_hedge.bucket10_30,0);
	//printf("spread1 %.3f spread2 %.3f spread3 %.3f spread4 %.3f\n",spread1,spread2,spread3,spread4);

	//set yc
	yc.set_arr(spread1/100,spread2/100,spread3/100,spread4/100);
	printf("After shift\n");
	//after shift
	dv01_2yr = yc.getdvo1(2);
	printf("dv01 after shift %.3f\n",dv01_2yr);	
	 double ShiftedR2,ShiftedM2,ShiftedH2,ShiftedR5,ShiftedM5,ShiftedH5,ShiftedR10,ShiftedM10,ShiftedH10,ShiftedR30,ShiftedM30,ShiftedH30;
	ShiftedH2 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket0_2,ShiftedR2,dv01_2yr,shift);
	ShiftedM2 = close_hedge.cal_bucket_mktval(close_hedge.bucket0_2,shift);
	ShiftedH5 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket2_5,ShiftedR5,dv01_2yr,shift);
	ShiftedM5 = close_hedge.cal_bucket_mktval(close_hedge.bucket2_5,shift);
	ShiftedH10 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket5_10,ShiftedR10,dv01_2yr,shift);
	ShiftedM10 = close_hedge.cal_bucket_mktval(close_hedge.bucket5_10,shift);
	ShiftedH30 = close_hedge.cal_2yr_hedge_amount(close_hedge.bucket10_30,ShiftedR30,dv01_2yr,shift);
	ShiftedM30 = close_hedge.cal_bucket_mktval(close_hedge.bucket10_30,shift);
    printf("spread1 %.3f spread2 %.3f spread3 %.3f spread4 %.3f\n",spread1,spread2,spread3,spread4);

	char *buffer= new char[16384];
	buffer[0]='\0';
	int n = sprintf(buffer,"closing_position,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;2yr_hedge,%.3f,%.3f,%.3f,%.3f;shifted_position,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;shifted_2yr_hedge,%.3f,%.3f,%.3f,%.3f;Yield_Curve,%.3f,%.3f,%.3f,%.3f;",R2,M2,R5,M5,R10,M10,R30,M30,H2,H5,H10,H30,ShiftedR2,ShiftedM2,ShiftedR5,ShiftedM5,ShiftedR10,ShiftedM10,ShiftedR30,ShiftedM30,ShiftedH2,ShiftedH5,ShiftedH10,ShiftedH30,yc2,yc5,yc10,yc30);
	//int n = sprintf(buffer,"closing_position,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;2yr_hedge,%.3f,%.3f,%.3f,%.3f;Yield_Curve,%.3f,%.3f,%.3f,%.3f;",Hedgerisk2,Mktval2,Hedgerisk5,Mktval5,Hedgerisk10,Mktval10,R30,Mktval30,Hedgeamount2,Hedgeamount5,Hedgeamount10,Hedgeamount30,Shifthedgerisk2,Shiftmktval2,Shifthedgerisk5,Shiftmktval5,Shifthedgerisk10,Shiftmktval10,Shifthedgerisk30,Shiftmktval30,Shifthedgeamount2,Shifthedgeamount5,Shifthedgeamount10,Shifthedgeamount30,yc2,yc5,yc10,yc30);
	//buffer[strlen(buffer)]='\0';
	//printf("buffer before dailychange:%s\n",buffer);
	char *daily = Hedging::dailychangebyissuer(&open_hedge,&close_hedge,shift);
	//printf("dailychange:%s\n",daily);
	strcat(buffer,daily);
	//printf("buffer after dailychange:%s\n",buffer);
	char * varout = Var::dailychangebyissuer(&Openvar,&Closevar,shift);
	//printf("varStr:%s\n",varout);
	strcat(buffer,varout);
	//printf("buffer now is:%s\n",buffer);

	
	
	//printf("buffer:%s\n",buffer);
	delete [] varout;
	delete [] daily;
	return buffer;
}

int main(int argc, char *argv[])
{
	if(argc != 5)
	{
		printf("testdriver opentrading closetrading yieldcurve\n");
		exit(0);
	}
	//SBB_util obj;
       double real_time,user_time,system_time;
       char dir[128];
		clock_t start = clock();

	   double cpu0  = get_cpu_time();

       SBB_instrument_input_file input_open(argv[1]);
       SBB_instrument_input_file input_closing(argv[2]);
       SBB_instrument_input_file yieldcurve(argv[3]);
	   strcpy(dir, argv[4]);
	//printf("dir is %s \n", &dir);
       int length=0;
       SBB_instrument_fields* yieldrecords=yieldcurve.records(length);
       Yieldcurve y(yieldrecords,length); 
       y.set_arr(0,0,0,0);
	   double dv01_2yr = y.getdvo1(2);          
       SBB_instrument_fields* open_records=input_open.records(length); 

       midoutput* res = new midoutput[length];
       int total_amount=0;
       int Amount;
       double total_risk=0.0;
       double risk=0.0; 
	   double price=0; 
	   double DV01=0; 
	   double lgd=0;
       int largest_long= 0, largest_short = 0;
	   double largest_risk = 0.0;

    //printf("length of file is %d \n",length);
    Instrument** records_calc_input = new Instrument*[length];

      


       for(int i=0;i<length;i++)
       {


			SBB_instrument_fields* bond_record_ptr = &open_records[i];
            static SBB_date settle_dt_obj, mat_dt_obj;
            settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate());
			mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate());
			int num_periods = SBB_term::get_num_periods(settle_dt_obj, mat_dt_obj,bond_record_ptr->Frequency());
		   //printf("Initialization done for record and num_periods %d,%d\n",i,num_periods);

			Instrument* bond_calc_ptr;
                     
			if(0.0 == bond_record_ptr->Coupon()) 
			{
                      
			 bond_calc_ptr=new ZeroCouponBond(bond_record_ptr,num_periods,&y);                      
			}
			else 	// must be coupon bearing - only two types in the file
			{
			 bond_calc_ptr=new CouponBond(bond_record_ptr,num_periods,&y);
			}
  		   
			//open_records[i].show();
			records_calc_input[i]=bond_calc_ptr;
			

	}	
	

	total_amount=0;
    total_risk=0.0;
    risk=0.0; 
	price=0; 
	DV01=0; 
	lgd=0;
    largest_long= 0;
	largest_short= 0;
	largest_risk = 0.0;
	int closing_length;
       SBB_instrument_fields* close_records=input_closing.records(closing_length); 
  
	Instrument** records_calc_output = new Instrument*[closing_length];

       for(int i=0;i<closing_length;i++)
       {


			SBB_instrument_fields* bond_record_ptr = &close_records[i];
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
			//close_records[i].show();
			records_calc_output[i]=bond_calc_ptr;
	}	
	


	/* 
	 * get an internet domain socket 
	 */
	int sd;
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/* 
	 * set up the socket structure 
	 */
	struct sockaddr_in	sock_addr;

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;

#ifdef SBB_ANY
	/* set to INADDR_ANY if want server to be open to any client on any machine */
	sock_addr.sin_addr.s_addr = INADDR_ANY;
#else
	char	hostname[128];
	/*
	 *  we'll default to this host and call a section 3 func to get this host
	 */
	if( gethostname(hostname,sizeof(hostname)) ){
		fprintf(stderr," SBB gethostname(...) failed errno: %d\n", errno);
		exit(1);
	}
	printf("SBB gethostname() local hostname: \"%s\"\n", hostname);

	/*
	 * set up socket structure for our host machine
	 */
	struct hostent *hp;
	if ((hp = gethostbyname(hostname)) == 0) {
		fprintf(stderr,"SBB gethostbyname(...) failed errno: %d exiting...\n", errno);
		exit(1);
	}
	sock_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
#endif
	sock_addr.sin_port = htons(PORT);
	printf("Port is %x \n",PORT);

	
	/* 
	 * bind the socket to the port number 
	 */

	if (bind(sd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1) {
		perror("bind");
		exit(1);
	}

	/* 
	 * advertise we are available on this socket/port
	 */
	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}

	/* 
	 * wait for a client to connect 
	 */
	struct sockaddr_in	sock_addr_from_client;
    socklen_t addrlen = sizeof(sock_addr_from_client); 
	int sd_current;
	if ((sd_current = accept(sd, (struct sockaddr *)  &sock_addr_from_client, &addrlen)) == -1) {
		fprintf(stderr,"SBB accept(...) failed errno: %d  exiting...\n", errno);
		exit(1);
	}
	printf("SBB client ip address: %s port: %x\n",inet_ntoa(sock_addr_from_client.sin_addr),PORT);

//		ntohs(sock_addr_from_client.sin_port));

	/*
	 * block on socket waiting for client message
	 */
	int ret = 0;
    char msg[MSGSIZE];
	char output[MSGSIZE];
	fprintf(stderr," SBB: sizeof msg: %lu\n", sizeof(msg));
	Hedging close_hedging(close_records, records_calc_output,closing_length);
    Hedging open_hedging(open_records, records_calc_input,length);
	Var varopening(dir,open_records,records_calc_input,length);
	Var varclosing(dir,close_records,records_calc_output,closing_length);
	while ( (ret = recv(sd_current, msg, sizeof(msg)-1, 0) > 0)) 
	{
       	printf("SBB server received msg: [%s] from client\n", msg);


		/* 
		 * ack back to the client 
		 */
		//obj.start_clock();
        
        
		//printf("Object created");

	   	
		char * result_str=gui_result(msg, y, open_hedging, close_hedging, varopening,varclosing);
		 clock_t end = clock();
		 double cpu1  = get_cpu_time();
		 real_time = (float)(end - start) / CLOCKS_PER_SEC;
		 user_time=cpu1  - cpu0;
		 system_time=user_time;
		//printf("\nresult_str is %s\n",result_str);
		memset(msg,'\0',sizeof(msg));
		sprintf(msg,"Server_Time,%.3f,%.3f,%.3f,%.3f;%s",real_time,user_time,system_time,user_time+system_time,result_str);
		//printf("Message sent is %s",msg);
		if (send(sd_current, msg, strlen(msg), 0) == -1)
		{
			fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
			exit(1);
		}
		memset(msg,0,sizeof(msg));

	}

	if( 0 == ret ) {
		printf("SBB ciient exited...\n");
		/* For TCP sockets	
		 * the return value 0 means the peer has closed its half side of the connection 
		 */
	}
	else if( -1 == ret ) {
		fprintf(stderr,"SBB recv(...) returned failed - errno: %d exiting...\n", errno);	
		exit(1);
	}
	close(sd_current); 
	close(sd);
	for (int i =0; i<length; i++)
	{
		delete records_calc_input[i];
	}
	for (int i =0; i<closing_length; i++)
	{
		delete records_calc_output[i];
	}
	delete [] records_calc_output;
	delete [] records_calc_input;
    input_open.free_records();
    input_closing.free_records();
    yieldcurve.free_records();
	
	return 0;
}

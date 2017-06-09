//============================================================================
// Name        : linker.cpp
// Author      : Vinayak Raghupathy
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================



#include <iostream>
#include <fstream>
#include<string>
#include<vector>
#include<sstream>
#include <cctype>
#include<map>

using namespace std;
struct uselist
{
string symbol;
int use_flag;
int module_cnt;

};
struct ins
{
	string var;
	int val;
    int addr;
    vector<string> use_list;
    int code_count;
    int mod_cnt;
    int last;

};
struct token
{
	string t;
	int line;
    int offset;
    string errmsg;

};

vector<token> v;
vector<ins>inst;
vector<int> module_address;
std::map<string, int> symbolTable;
std::map<string,int> token_module;
std::map<string,string> symbol_error;
std::map<string,int>symbol_used;
vector<string>total_use_list;
vector<uselist>use_list_final;
vector<string> insert_order;

int total_lines=0;
int check_symbol(string content)
{
int size=content.length();
if (!isalpha(content[0])) {
       return -1;
   }
for (int k=1;k<size;k++)
{
if(!isalnum(content[k]))
{
  	return -1;
}
}
return 1;
}

int count_digit(int x)
{

	int length = 1;

	while ( x /= 10 )
	   length++;
return length;
}



int check_number(string content)
{
  int size=content.length();
  int result=0;
  for (int k=0;k<size;++k)
  {
	  if(isdigit(content[k]))
	  { result=result*10+content[k] -'0';
	  }
	  else
	  {
		result=-1;
		break;
	  }
  }

return result;
}

void parseerror(int errcode,int linenum,int lineoffset)
{
static string errstr[] = {
"NUM_EXPECTED",
"SYM_EXPECTED",
"ADDR_EXPECTED",
"SYM_TOLONG",
"TO_MANY_DEF_IN_MODULE",
"TO_MANY_USE_IN_MODULE",
"TO_MANY_INSTR"
};
cout<<"Parse Error line "<< linenum << " offset " << lineoffset <<": "<<errstr[errcode];
}
void print_warnings()
{
    //std::map<string,int>::iterator it = symbolTable.begin();

    int flag;
	for (int c=0;c<insert_order.size();c++)



		  {   string sym=insert_order[c] ; flag=0;
		  	
		  map<string,int>::iterator it = symbol_used.find(sym);

		  if(it != symbol_used.end())
		  {

		  if (symbol_used.find(sym)->second==0)
		  {

			  printf("Warning: Module %d: ",token_module[sym]);
			 				  cout<<sym<<" was defined but never used"<<endl;
		  }

		  }
		  }

}


int memory_map()
{
	cout<<"Memory Map"<<endl;
    string warn="";
    int module=1;
	for(int i=0;i<inst.size();i++)
	{
		//cout<<"Mod count for instruction is"<<inst[i].mod_cnt<<endl;
	   //cout<<"Module count  is"<<module<<endl;
	   string errmsg="";

        if(inst[i].var=="R")

        {
        	int op=(inst[i].val)/1000;

            int code=inst[i].val%1000;

              if (inst[i].val>9999)
             {
            	  errmsg="Error: Illegal opcode; treated as 9999";
        	      printf("%03d: %d ",i,9999);
        	      cout<<errmsg;
                  cout<<endl;
             }



               else  if(code>=inst[i].code_count)
               {

        	    int res=(op*1000)+inst[i].addr;
               	errmsg="Error: Relative address exceeds module size; zero used";
               	printf("%03d: %04d ",i,res);
                cout<<errmsg;
                cout<<endl;
               }

                else  if(inst[i].val<=9999)
                {
                	printf("%03d: %04d",i,inst[i].val+inst[i].addr);
                    cout<<endl;
                }
           }
     else if(inst[i].var=="E")
     {
                int op=(inst[i].val)/1000;

                int code=(inst[i].val)%1000;

                if (code>inst[i].use_list.size()-1)
           	    {
           		  errmsg="Error: External address exceeds length of uselist; treated as immediate";
           		  printf("%03d: %04d ",i,inst[i].val);
           		  cout<<errmsg<<endl;
                 }
           	 else
           	 {
                  for(int a=0;a<use_list_final.size();a++)
                  {
                      if(code<inst[i].use_list.size())
                	  {
                    	  // if(!inst[i].use_list[code].empty()||!use_list_final[a].symbol.empty())
                    	   //{
                    	  if(use_list_final[a].symbol==inst[i].use_list[code])

                	  {
                		  use_list_final[a].use_flag=1;
                	  }
                	  //}
                  }
                  }
                   if(inst[i].use_list.size()>0)
                   {if (symbolTable.count(inst[i].use_list[code])==0)
                   {
                	printf("%03d: %04d ",i,inst[i].val);
                   	cout<<"Error: "<<inst[i].use_list[code]<<" is not defined; zero used"<<endl;
                   }
                   else
                   {
                	   printf("%03d: %04d",i,(op*1000)+symbolTable[inst[i].use_list[code]]);
                	   cout<<endl;
                   }}

                 	 }

     }



     else if(inst[i].var=="I")
     {
         if (inst[i].val<=9999)
    	 {
    	 printf("%03d: %04d",i,inst[i].val);
    	 cout<<endl;
    	 }
    	 else if(inst[i].val>9999)
    	 {

         errmsg="Error: Illegal immediate value; treated as 9999";
         printf("%03d: %04d ",i,9999);
         cout<<errmsg;
         cout<<endl;
    	 }

     }
     else if (inst[i].var=="A")
     {    int code=inst[i].val%1000;
           int op=(inst[i].val)/1000;
         if(code>512)
         { errmsg="Error: Absolute address exceeds machine size; zero used";
           printf("%03d: %04d ",i,op*1000);
           cout<<errmsg;
           cout<<endl;

         }
         else if (inst[i].val<=9999)
    	    	 {
    	    	 printf("%03d: %04d",i,inst[i].val);
    	    	 cout<<endl;
    	    	 }
    	    	 else
    	    	 {

    	         errmsg="Error: Illegal opcode; treated as 9999";
    	         printf("%03d: %04d ",i,inst[i].val);
    	         cout<<errmsg;
    	         cout<<endl;
    	    	 }

     }
            int current_module=inst[i].mod_cnt;
            for (int k=0;k<use_list_final.size();k++)
        	 {
        		 if(use_list_final[k].use_flag==0 && use_list_final[k].module_cnt==current_module )
        		  {ostringstream oss;
        		   oss<< use_list_final[k].module_cnt;
        		   warn="Warning: Module "+oss.str()+": "+use_list_final[k].symbol+" appeared in the uselist but was not actually used";
        		   if(inst[i].last==1 )
        		        {
                         current_module++;
        			      if(!warn.empty())
        		      	  {cout<<warn;
        		      	  cout<<endl;
        		      	  }
        		        }
        		  }
        		 else{
        			 warn="";
        		 }

        	 }

            int code=inst[i].val%1000;


            for(int a=0;a<use_list_final.size();a++)
                              {
                if(inst[i].use_list.size()>0)
            	{if(!inst[i].use_list.empty()||!use_list_final.empty())
            		                  if(code<inst[i].use_list.size())
            		                  {
                                { if(use_list_final[a].symbol.compare(inst[i].use_list[code])==0)
                            	  {   if(symbol_used.count(use_list_final[a].symbol)>0)
                            	  {
                            		  symbol_used[use_list_final[a].symbol]=1;
                            	  }

                            	  }}}
                              }
                              }





	}

return 0;
}







int parse()
{   int initial=0;string errmsg="",val="",mode="";
    std::map<string,int>::iterator it = symbolTable.begin();
    std::map<string,string>::iterator it_err = symbol_error.begin();
    std::map<string,int>::iterator it_token = token_module.begin();
    std::map<string,int>::iterator it_used = symbol_used.begin();

    int module_count=0;
	int cnt_module=0;
    int def_count,inst_count=0;


while(!v.empty())
{
	vector<string> use_list;
	cnt_module=cnt_module+1;
       token t1=v.front();
       v.erase(v.begin());
       def_count=check_number(t1.t);
       //cout<<"def_count is"<<def_count<<endl;
       if (def_count==-1)
       {
	parseerror(0,t1.line,t1.offset);
	return -1;

       }

if(v.size()==0 && def_count==0)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
if(v.size()==0 && def_count!=0 && def_count<16)
{      //cout<<"total_line is"<<total_lines<<endl;
	if(total_lines<=2)
	{
	parseerror(1,t1.line+total_lines-1,t1.offset+count_digit(check_number(t1.t)));
	return -1;
	}
	else
	{

		    parseerror(1,t1.line+total_lines-1,1);
	           return -1;

	}
}
else if(def_count>16)
{
	parseerror(4,t1.line,t1.offset);
	return -1;

}



for (int p=0;p<def_count;p++)
{
t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(1,t1.line,t1.offset);
return -1;
}
if(!check_symbol(t1.t))
{
	parseerror(1,t1.line,t1.offset);
       return -1;
}
else if(t1.t.length()>16)
{
	parseerror(3,t1.line,t1.offset);
	return -1;

}
string symbol=t1.t;
t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
int value=check_number(t1.t);
if(value==-1)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
 //
if (symbolTable.count(symbol)==0)
{

symbolTable.insert(it,std::pair<string,int>(symbol,value+inst_count));
symbol_error.insert(it_err,std::pair<string,string>(symbol,""));
insert_order.push_back(symbol);
symbol_used.insert(it_used,std::pair<string,int>(symbol,0));
}
else
{

	errmsg="Error: This variable is multiple times defined; first value used";
	symbol_error[symbol]=errmsg;
}

if(token_module.count(symbol)==0)
{
	token_module.insert(it_token,std::pair<string,int>(symbol,cnt_module));

}





}

t1=v.front();
v.erase(v.begin());
int use_count=check_number(t1.t);

if (use_count==-1)
{
	parseerror(0,t1.line,t1.offset);
	return -1;

}
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
if(use_count>16)
{
	parseerror(5,t1.line,t1.offset);
	return -1;

}

//cout<<"use_count is"<<use_count<<endl;
for (int q=0;q<use_count;q++)
{
t1=v.front();
v.erase(v.begin());

if(v.size()==0)
{
parseerror(1,t1.line,t1.offset);
return -1;
}
if(!check_symbol(t1.t))
{
	parseerror(1,t1.line,t1.offset);
       return -1;
}
else if(t1.t.length()>16)
{
	parseerror(3,t1.line,t1.offset);
	return -1;

}
total_use_list.push_back(t1.t);
use_list.push_back(t1.t);
uselist  u;
u.symbol=t1.t;
u.use_flag=0;
u.module_cnt=cnt_module;

use_list_final.push_back(u);


}


t1=v.front();
v.erase(v.begin());
int code_count=check_number(t1.t);
if (code_count==-1)
{
	parseerror(0,t1.line,t1.offset);
	return -1;

}
if (code_count+inst_count>512)
{

   	 parseerror(6,t1.line,t1.offset);
	 return -1;

}

if(v.size()==0)
{
parseerror(2,t1.line,t1.offset + count_digit(check_number(t1.t)) );
return -1;
}

module_address.push_back(initial);

module_count++;

//cout<<"code_count is"<<code_count<<endl;
initial=initial+code_count;


int res=code_count;
for (int r=0;r<res;r++)
{
	inst_count++;
	//cout<< "r = " <<r<<endl;
	t1=v.front();
	v.erase(v.begin());
	if(v.size()==0)
	{
		if(!check_number(t1.t))
		{
			parseerror(2,t1.line,t1.offset);
			return -1;
		}
	}
	if (t1.t!="I"&&t1.t!="A"&&t1.t!="R"&&t1.t!="E")
	{
		parseerror(2,t1.line,t1.offset);
		return -1;
	}
	else
	{
		mode=t1.t;
	}

	t1=v.front();
	v.erase(v.begin());

	if(v.size()==0 && !check_number(t1.t))
	{
		parseerror(0,t1.line,t1.offset);
		return -1;
	}

	int addr=check_number(t1.t);
	ins in;

	in.var=mode;
	in.val=addr;
	in.addr=module_address.front();
	in.mod_cnt=cnt_module;
	if(r==res-1)
	{
		in.last=1;
	}
	else
	{
		in.last=0;
	}
	if(in.var=="R")
	{
		in.code_count=code_count;
	}
	if(in.var=="E")
	{
		for(int q=0;q<use_list.size();q++)
		{
			in.use_list.push_back(use_list[q]);
		}
	}
	module_address.clear();
	inst.push_back(in);
	//symbolTable[t1.t] = t1.offset + module_address[i];

}



	for (int c=0;c<insert_order.size();c++)
	{
		   string sym=insert_order[c];

           if (symbolTable.find(sym)->second >= inst_count)
           {

        	   cout << "Warning: Module " << module_count << ": " << sym << " to big " <<symbolTable.find(sym)->second -inst_count+code_count<< " (max=" <<code_count - 1 << ") assume zero relative" << endl;
                     symbolTable[sym] = inst_count-code_count;
           }
     }

     }


cout<<"Symbol Table"<<"\n";
for (int b=0;b<insert_order.size();b++)
{

   string sym=insert_order[b];
   cout << sym << "=" << symbolTable.find(sym)->second<<" "<<symbol_error[sym]<<endl;

}
/*for ( it=symbolTable.begin(); it!=symbolTable.end(); ++it)

	  {
	    //cout << it->first << "=" << it->second <<" "<<it->first.errmsg<<endl;
	      cout << it->first << "=" << it->second<<" "<<symbol_error[it->first]<<endl;
	  }*/

return 0;
}

int main(int argc,char * argv[]) {


	 string content,word="";
	 //cout<<"Number of arguments is "<<argc<<endl;
	 //cout<<"The list is "<<argv[1]<<endl;
	  if (argc == 1)
	  {
	    cout << "Please enter file name" << endl;
	    return -1;
	   }
 	  ifstream myfile(argv[1]);
	  if (freopen(argv[1], "r", stdin))
	  {     int i=0; 

		  while(getline(myfile,content))
		  {   total_lines++;
			  stringstream s1(content);
			  i=i+1;
			  while( s1 >> word )
				  {      token s2;
				         s2.line=i;
                         s2.t=word;
                         s2.offset=content.find(word) + 1;
                         v.push_back(s2);

				  }



           }


          		  myfile.close();

		  }
	  else
	  {
		  cout<<"Error opening the file"<<endl;
		  return -1;

	  }

      int res= parse();
      cout<<endl;
      if(res!=-1)
      {
      res=memory_map();
      cout<<endl;
      print_warnings();
      }
      else
      {
    	  return -1;

      }


  return 0;
}
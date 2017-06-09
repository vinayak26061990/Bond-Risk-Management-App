//============================================================================
// Name        : test.cpp
// Author      : 
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
struct token
{
	string t;
	int line;
	int offset;
}t;

vector<token> v;
vector<int> module_address;
std::map<string, int> symbolTable;
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


int check_number(string content)
{
  int size=content.length();
  int result;
  for (int k=0;k<size;k++)
  {
	  if(isdigit(content[k]))
	  { result=result*10+content[k] -'0';
	  }
	  else
	  {
		return -1;
	  }
  }

  return result;
}


void parseerror(int errcode,int linenum,int lineoffset)
{
static char* errstr[] = {
"NUM_EXPECTED",
"SYM_EXPECTED",
"ADDR_EXPECTED",
"SYM_TOLONG",
"TO_MANY_DEF_IN_MODULE",
"TO_MANY_USE_IN_MODULE",
"TO_MANY_INSTR"
};
cout<<"Parse Error line"<< linenum << "offset" << lineoffset <<":"<<errstr[errcode];
}
int parse()
{
	std::map<string,int>::iterator it = symbolTable.begin();

	int def_count,len,hash_code,inst_count=0;

while(!v.empty())
{

token t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
def_count=check_number(t1.t);
if(def_count>16)
{
	parseerror(4,t1.line,t1.offset);
	return -1;

}
else if (def_count==-1)
{
	parseerror(0,t1.line,t1.offset);
	return -1;

}
for (int p=0;p<def_count;p++)
{
t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
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

symbolTable.insert(it,std::pair<string,int>(symbol,value+inst_count));

}

t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
int use_count=check_number(t1.t);
if(use_count>16)
{
	parseerror(4,t1.line,t1.offset);
	return -1;

}
else if (use_count==-1)
{
	parseerror(0,t1.line,t1.offset);
	return -1;

}

for (int q=0;q<use_count;q++)
{
t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
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
}


t1=v.front();
v.erase(v.begin());
if(v.size()==0)
{
parseerror(0,t1.line,t1.offset);
return -1;
}
int code_count=check_number(t1.t);
if(code_count>16)
{
	parseerror(4,t1.line,t1.offset);
	return -1;

}
else if (code_count==-1)
{
	parseerror(0,t1.line,t1.offset);
	return -1;

}

for (int r=0;r<code_count*2;r++)
{
t1=v.front();
v.erase(v.begin());
if(v.size()==0 && (check_number(t1.t)==-1))
{
parseerror(0,t1.line,t1.offset);
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
if(r<code_count)
{
	inst_count++;
}

//symbolTable[t1.t] = t1.offset + module_address[i];

}
}
cout<<"Symbol table"<<"\n";
for ( it=symbolTable.begin(); it!=symbolTable.end(); ++it)
	  {  cout << it->first << " => " << it->second << '\n';
	  }

return 0;
}

int main(int argc,char * argv[]) {


	 string content,word;
	 cout<<"Number of arguments is "<<argc<<endl;
	 cout<<"The list is "<<argv[1]<<endl;

 	  ifstream myfile(argv[1]);
	  if (myfile.is_open())
	  {   int i=0;

		  while(getline(myfile,content))
		  {
			  stringstream s1(content);
			  i=i+1;
			  while( s1 >> word )
				  {      token s2;
				         s2.line=i;
                         s2.t=word;
                         s2.offset=content.find(word);
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


 int  res=parse();


  return 0;
}











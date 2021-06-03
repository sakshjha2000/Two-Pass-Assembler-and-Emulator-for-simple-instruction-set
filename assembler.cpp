/*NAme - Saksham Jha 
   Roll number -1801cs64*/
#include<bits/stdc++.h>
using namespace std;

//Below are the data structures which stores the values in different format
map<int,string>warnings;	//stores warnings
map<int,string>errors;		//stores errors
map < string, pair<string, int> >opcode;	//stores opcode for all instructions in a partiular format
map<int , pair<string, pair<string,string> > >lineinfo;	//this stores info of each line in format lineinfo[prctr]={label,{mnemonic,operand}};
map<string,vector<int> >labelpos;	//this stores location of each label
//this stores instruction in format which is required in lst file
struct listing {
    string prctr,mcode,text;
};
//this stores the machine code for each instruction
vector<string>machinecode;
//this stores data for lst file
vector<listing>listi;
void storeopcodes()
{
    /*We store the opcodes in a format where key is opcode and it points to a pair
    	where first part refers to conversion of opcode in hex and second part refers to type of opcode
    	requiring offset or value or nothing*/
    opcode["data"]={"",2};
    opcode["ldc"]={"00",2};
    opcode["adc"]={"01",2};
    opcode["ldl"]={"02",3};
    opcode["stl"]={"03",3};
    opcode["ldnl"]={"04",3};
    opcode["stnl"]={"05",3};
    opcode["add"]={"06",1};
    opcode["sub"]={"07",1};
    opcode["shl"]={"08",1};
    opcode["shr"]={"09",1};
    opcode["adj"]={"0a",2};
    opcode["a2sp"]={"0b",1};
    opcode["sp2a"]={"0c",1};
    opcode["call"]={"0d",3};
    opcode["return"]={"0e",1};
    opcode["brz"]={"0f",3};
    opcode["brlz"]={"10",3};
    opcode["br"]={"11",3};
    opcode["HALT"]={"12",1};
    opcode["SET"]={"",2};
}
map<string , pair<int,int> >labeltable;
//this data structure is used to store values for set instruction
map<string,string>setvar;
//The following class contains different kind of checker functions
class checker {
public:
    //check digit
    int digit(char c)
    {
        if(c>='0' && c<='9')
            return 1;
        else
            return 0;
    }
    //check alphabet
    int alphabet(char c)
    {
        if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')
            return 1;
        else
            return 0;
    }
    //check decimal
    int decimal(string s)
    {
        int ch=1;
        if(s[0]=='0' && s.size()!=1)
            ch=0;
        for(int i=1; i<s.size(); i++)
        {
            if(s[i]>='0' && s[i]<='9')
                continue;
            else
            {
                ch=0;
                break;
            }
        }
        return ch;
    }
    //check octal
    int octal(string s)
    {
        int ch=1;
        int siz=s.size();
        if(siz<2 || s[0]!='0')
            return 0;
        for(int i=1; i<s.size(); i++)
        {
            if(s[i]>='0' && s[i]<='7')
                continue;
            else
                ch=0;
        }

        return ch;
    }
	//check hexadecimal
    int hexadecimal(string s)
    {
        int ch=1;
        int siz=s.size();
        if(siz<3 || s[0]!='0' || tolower(s[1])!='x')
            return 0;
        for( int i=2; i<s.size(); i++)
        {
            if((s[i]>='0' && s[i]<='9') || (tolower(s[i])>='a' && tolower(s[i])<='f') )
                continue;
            else
                ch=0;
        }
        return ch;
    }
//checks if a string is a valid label
    int label(string s)
    {
        int siz=s.size();
        if(siz==0)
            return 0;
        int ch=1;
        if(!alphabet(s[0]))
            ch=0;
        for(int i=1; i<siz; i++)
        {
            if(alphabet(s[i]) || digit(s[i]))
                continue;
            else
                ch=0;
        }
        return ch;

    }


};
checker chk;

//Following class contains different types of converter functions which converts from one base to another
class converter {
public:
    string octdec(string s)
    {
        int res=0,pow=1;
        int siz=s.size();
        for(int i=siz-1; i>=0; i--)
        {
            res+=(pow*(s[i]-'0'));
            pow*=8;
        }

        string dec=to_string(res);
        return dec;
    }

    string hexdec(string s)
    {
        int res=0,pow=1;
        int siz=s.size();
        for(int i=siz-1; i>=0; i--)
        {
            if(chk.digit(s[i]))
            res+=(pow*(s[i]-'0'));
            else
                res+=(pow* ((tolower(s[i])-'a') +10));
            pow*=16;
        }

        string dec=to_string(res);
        return dec;
    }

    string dechex(int num)
    {
        unsigned int num1=num;
        string res="";
        for(int i=0; i<8; i++)
        {
            int rem=num1%16;
            if(rem<=9)
            {
                char ch =rem +'0';
                res+=ch;
            }
            else
            {
                char ch=rem-10 +'a';
                res+=ch;
            }
            num1/=16;
        }
        reverse(res.begin(),res.end());
        return res;
    }
};
converter conv;

//This class contains some extra functions which we require during pass1
class solver {
public:
	//checks for some errors and stores labelpositions
    void solvelabel(string labe,int prctr,int linenum)
    {
        int siz=labe.size();
        if(siz==0)
            return;
        bool valid=chk.label(labe);
        if(!valid)
        {
            errors[linenum]="Bogus Label name";
        }
        else
        {
            if(labeltable.find(labe)!=labeltable.end() && labeltable[labe].first!=-1)
            {
                    errors[linenum]="Duplicate Label Definition";
            }
            else
                labeltable[labe]={prctr,linenum};
        }
    }
	//gets the decimal conversion of number
    string solveoperand(string temp)
    {
	
        string num="";
        if(chk.octal(temp))
        {
            temp=temp.substr(1);
            string dec=conv.octdec(temp);
            num+=dec;
        }
        else if(chk.hexadecimal(temp))
        {
            temp=temp.substr(2);
            string dec=conv.hexdec(temp);
            num+=dec;
        }
        else if(chk.decimal(temp))
        {
            num+=temp;
        }
        return num;
    }
	//checks for some errors as well as returns 1 if a instruction is valid and 0 if not valid
    int solvemnemonic(string mnemonic,string &operand, int prct, int linenum)
    {
        int typ=opcode[mnemonic].second;
        int sizofoperand=operand.size();
        if(typ>1)
        {
                string s;
                if(chk.label(operand))
        	{
            		if(labeltable.find(operand)==labeltable.end())
                		labeltable[operand]={-1,linenum};
            		labelpos[operand].push_back(linenum);
            		s=operand;
            		
        	}
        	else
        	{
        		s="";
        		if(operand[0]=='+' || operand[0]=='-')
        		{
        			s+=operand[0];
        			s+=solveoperand(operand.substr(1));
        		}
        		else
        		{
        			s+=solveoperand(operand);
        		}        		
        	}
        	
                if(s.size()==0)
                {
                errors[linenum]="Invalid Format";
                return 0;
                }
                else
                {
                    operand=s;
                    return 1;
                }

        }
        else if(typ==1 && sizofoperand!=0)
        {
            errors[linenum]="Unexpected operand";
            return 0;
        }
        else
            return 1;

    }


};
solver sol;
//This function extracts words from each line and returns it in format of vector of strings
vector<string>getwordfromline(string line)
{
    int siz=line.size();
    if(siz==0)
        return {};
    vector<string>words;

    stringstream curword(line);
    string word;
    while(curword >> word)
    {
            int wordsiz=word.size();
            if(wordsiz==0)
                continue;
            if(word[0]==';')
                break;
            auto i = word.find(':');
            if (i != string::npos and word.back() != ':')
            {
                words.push_back(word.substr(0, i + 1));
                word = word.substr(i + 1);
            }
            if (word.back() == ';')
            {				// case when there is no seperation between ';' and previous word
			word.pop_back();
			words.push_back(word);
			break;
            }
            words.push_back(word);

        
    }

    return words;
}
//This stores the content from file in which each string is a line
vector<string>inputline;
void pass1()
{
    
    int curline=0,prctr=0;
    for (string linewise: inputline)
    {
        curline++;
        auto cur = getwordfromline(linewise);
        if(cur.size()==0)
            continue;
        int it=0,siz=cur.size();
        //Assigning different parts of a line
        string labe="",mnemonic="",operand="";
        if(cur[it].back()==':')
        {
            labe=cur[it];
            labe.pop_back();
            it++;
        }
        if(it<siz)
        {
            mnemonic=cur[it];
            it++;
        }
        if(it<siz)
        {
            operand=cur[it];
            it++;
        }
        //checking for some errors and updating program counter
        sol.solvelabel(labe,prctr,curline);
        int cnt=0;
        if(mnemonic.size()!=0)
        {
        	if(opcode.find(mnemonic)==opcode.end())
        	{
            		errors[curline]="Bogus Mnemonic";
        	}
        	else
        	{
        		if(opcode[mnemonic].second>1)
        		{
        			if(operand.size()==0)
        				errors[curline]="Missing operand";
        			else if(siz-it>0)
        				errors[curline]="Extra on end of line";		
        		}
        	}
        	
        	cnt=sol.solvemnemonic(mnemonic,operand,prctr,curline);
        	//cout<<operand<<"\n";
        	lineinfo[prctr]={labe,{mnemonic,operand}};
        	prctr+=cnt;	//program counter being updated
        }
        else
        	lineinfo[prctr]={labe,{mnemonic,operand}};
        //extra info required for set instruction
       if (cnt && mnemonic == "SET") 
       {		
		if (labe.empty()) 
		{
			errors[curline]= "label name or variable name missing";
		} 
		else 
		{
			// Storing SET instruction information
			setvar[labe] = operand;		
		}
	}

    }
    //checking for some more errors
    for(auto it: labeltable)
    {
            if(it.second.first==-1)
            {
                for(auto linenum: labelpos[it.first])
                    errors[linenum]="No such label";
            }
            else if(!labelpos.count(it.first))
            {
                warnings[it.second.second]="Label declared but not used";
            }
    }


}

//this function is for pass 2
void pass2()
{
    for (auto it : lineinfo)
    {
    	//Extracting info which we stored in pass1
        int pctr=it.first;
        pair<string , pair<string,string> >pa =it.second;
        string label=pa.first;
        string mnemonic=pa.second.first;
        string operand=pa.second.second;
        int type =-100;
        if(!mnemonic.empty())
            type=opcode[mnemonic].second;
        string mcode="       ";
        //Based on the type of instruction assigning the machine codes
        switch (type)
        {
        	case 1: 
        		mcode="000000" + opcode[mnemonic].first;
        		break;
        	
        	case 2:
        		if(mnemonic=="data" || mnemonic=="SET")
        			mcode=conv.dechex(stoi(operand));
        		else
        		{
        			int val;
        			if(!labeltable.count(operand))
            			{
                			val=(int)stoi(operand);
                			//cout<<operand<<"\n";
            			}	
            			else
                			val=labeltable[operand].first;
            			mcode=conv.dechex(val).substr(2) +opcode[mnemonic].first;
            			if (setvar.count(operand)) 
            			{
					// if in case the operand is a variable used in SET operation
					mcode = conv.dechex(stoi(setvar[operand])).substr(2) + opcode[mnemonic].first;
				}
        		}
        		break;
        	case 3:
        		int offset;
            		if(labeltable.find(operand)==labeltable.end())
            		{
                		offset=(int)stoi(operand);
            		}
            		else
                		offset=labeltable[operand].first-(pctr+1);
            		mcode=conv.dechex(offset).substr(2) +opcode[mnemonic].first;
        		break;
        	default :
        		break;
        		
        }
        /*if(mnemonic=="ldc" && operand=="0x1000)
        	cout<<mcode<<"\n";*/
        machinecode.push_back(mcode);
        if (!label.empty())
            label += ": ";
        if (!mnemonic.empty())
            mnemonic += " ";
        string text = label + mnemonic + operand;	//this makes the format for writing in lst files
        //cout<<mcode<<"\n";
        listi.push_back({conv.dechex(pctr), mcode, text});
    }
}

//This function writes into the logfile if errors are there
void writeerrors()
{
    ofstream printerrors("logfile.log");
    if(errors.size()==0)
    {
    	printerrors<<"HURRAY NO ERRORS";
        for (auto it: warnings)
        {
            printerrors<<"Line Number "<<it.first<<" WARNING: "<<it.second<<"\n";
        }
        printerrors.close();
        return;
    }

    for(auto it: errors)
    {
        printerrors<<"Line Number "<<it.first<<" ERROR: "<<it.second<<"\n";
    }
    printerrors.close();
}
int main()
{
    storeopcodes();
	//Taking input from file
    ifstream inpfile;
	inpfile.open("input.txt");
	if (inpfile.fail())
    {
		cout << "Input file doesn't exit" <<"\n";
    }
    else
    {
        string lines;
        while (getline(inpfile, lines))
        {
		inputline.push_back(lines);
        }
    }
	inpfile.close();
	//PAss1
    pass1();
    writeerrors();
    int siz=errors.size();
    if(siz==0)
    {
    	//If no errors are there then we do pass2
        pass2();
	//Writing in lst file
        ofstream writelist("listfile.lst");
        for(auto it: listi)
        {
            writelist<< it.prctr<<" "<<it.mcode<<" "<<it.text<<"\n";
        }
        cout<<"errorfile generated"<<"\n";
        cout<<"Listfile generated"<<"\n";
        writelist.close();
        //writing in .o file
        ofstream printmcode;
        printmcode.open("machinecode.o", ios::binary | ios::out);
        for (auto code : machinecode)
        {
            if (code.empty() || (code == "        "));
                continue;
            unsigned int cur = (unsigned int)stoi(conv.hexdec(code));
            static_cast<int>(cur);
            printmcode.write((const char*)&cur, sizeof(unsigned int));
        }
        printmcode.close();
        cout<<"Machinecode file generated"<<"\n";
        

    }
    return 0;
}

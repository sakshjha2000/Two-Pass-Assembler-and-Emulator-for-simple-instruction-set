/*Name - Saksham Jha
Roll number -1801cs64*/

#include<bits/stdc++.h>
using namespace std;

//This function converts the decimal to hex and returns tring
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
//this array stores the array content
int mem[20000005];
int pc,sp,a,b;	//stores the value of 4 registers
vector<int>machinecodes;//stores the machinecodes
struct reading{
	int add;
	int value;
};
vector<reading>read1;	//stores memory read 

struct writing {
	int add;
	int previous;
	int present;

};
vector<writing>write1;	//stores memory write
//Follwing function traces the .o file whole and last tells total instruction or segmentation fault
void trace(int siz)
{
	pc=0;
	int cnt=0;
	bool chk=false,chk1=false;
	while(pc<siz)
	{
		

		int instr=machinecodes[pc];
		//We break the machine code to offset and opcode offset is same as value
		int type=0;
		for(int i=0; i<8; i++)
		{
			if(instr&(1<<i))
			type+=(1<<i);
		}
		int offset=0;
		for(int i=8; i<31; i++)
		{
			if(instr & (1<<i))
				offset+=(1<<(i-8));

		}
		if(instr &(1<<31))
			offset-=(1<<(23));
		//cout<<type<<" "<<offset<<"\n";
		int prev=-1;
		//If else statements to check waht needs to be done
		switch(type)
		{
			case 0:
					b=a;
					a=offset;
					break;
			case 1:
					a=a+offset;
					break;
			case 2:
					b=a;
					a=mem[sp +offset];
					read1.push_back({sp+offset,a});
					break;
			case 3:
					 prev=mem[sp+offset];
					mem[sp+offset]=a;
					a=b;
					write1.push_back({sp+offset,prev,mem[sp+offset]});
					break;
			case 4:
					 prev=a;
					a=mem[a+offset];
					read1.push_back({prev+offset,a});
					break;
			case 5:
					 prev=mem[a+offset];
					mem[a+offset]=b;
					write1.push_back({a+offset,prev,b});
					break;
			case 6:
					a=b+a;
					break;
			case 7:
					a=b-a;
					break;

			case 8:
					a=(b<<a);
					break;

			case 9:
					a=(b>>a);
					break;

			case 10:
					sp=sp+(offset);
					break;

			case 11:
					sp=a;
					a=b;
					break;

			case 12:
					b=a;
					a=sp;
					break;
			case 13:
					b=a;
					a=pc;
					pc=(pc+offset);
					break;

			case 14:
					pc=a;
					a=b;
					break;

			case 15:
					if(a==0)
					{
						pc=(pc+offset);
					}
					break;

			case 16:
					if(a<0)
					{
						pc=(pc+offset);
					}
					break;

			case 17:
					pc=(pc+offset);
					break;

			case 18:
					chk=true;
					break;

		}
		
		pc++;
		cout<<"PC= "<<dechex(pc)<<" SP= "<<dechex(sp)<<" A= "<<dechex(a)<<" B= "<<dechex(b)<<"\n";
		//If infinite loop or jump to wrong instruction exit
		if(pc<0 || cnt> (1<<24))
		{
			chk1=true;
			break;
		}
		cnt++;
		if(chk)
			break;
		//cout<<cnt<<"\n";
	}
	if(chk1)
	{
		cout<<"Segmentation fault or some other error"<<"\n";
		exit(0);
	}
	cout<<"Total instructions ran are "<<"\n";
	cout<<cnt<<"\n";
	chk=false;
}
//Following function displays all possible commands
void showisa()
{
	cout<<"Opcode Mnemonic Operand"<<'\n';
	cout<<"0      ldc      value "<<"\n";
	cout<<"1      adc      value "<<"\n";
	cout<<"2      ldl      value "<<"\n";
	cout<<"3      stl      value "<<"\n";
	cout<<"4      ldnl     value "<<"\n";
	cout<<"5      stnl     value "<<"\n";
	cout<<"6      add            "<<"\n";
	cout<<"7      sub            "<<"\n";
	cout<<"9      shr            "<<"\n";
	cout<<"10     adj      value "<<"\n";
	cout<<"11     a2sp           "<<"\n";
	cout<<"12     sp2a           "<<"\n";
	cout<<"13     call     offset"<<"\n";
	cout<<"14     return         "<<"\n";
	cout<<"15     brz      offset"<<"\n";
	cout<<"16     brlz     offset"<<"\n";
	cout<<"17     br       offset"<<"\n";
	cout<<"18     HALT           "<<"\n";
	cout<<"       SET      value "<<"\n";
}
int32_t main()
{
	//Taking input of .o file
	string nameoffile;
	cout<<"Enter the File name for which you want to run emulator for(write name exactly with extension)"<<"\n";
	cin >>nameoffile;
	ifstream file(nameoffile, ios::in | ios::binary);
	unsigned int cur;
	int counter = 0;
	//Reading file and storing machine codes
	while(file.read((char*)&cur, sizeof(int))) { 
		machinecodes.push_back(cur);
		//cout<<cur<<"\n";
		mem[counter++] = cur;
	}
	int siz=machinecodes.size();
	//Menu driven program which gives you some functions to perform
	while(1) {
	cout<<"MENU"<<"\n";
	cout<<"1: To get trace"<<"\n";
	cout<<"2: To display ISA"<<"\n";
	cout<<"3: To wipe out all the flags"<<"\n";
	cout<<"4: To show memory dump before execution(don't use this execution after rrunning trace)"<<"\n";
	cout<<"5: to show memory dump after execution(don't use this instruction before running trace)"<<"\n";
	cout<<"6: Show memory reads"<<"\n";
	cout<<"7: Show memory writes"<<"\n";
	cout<<"Any number >=8 to exit"<<"\n";
	cout<<"\n";
	cout<<"Enter the type of instruction which you want to do"<<"\n";
	int type;
	cin>>type;
	if(type==1)
	{
		trace(siz);
	}
	else if(type==2)
	{
		showisa();
	}
	else if(type==3)
	{
		a=b=pc=sp=0;
	}
	else if(type==4)
	{
		cout << "Memory dump before execution" << endl;
		for (int i=0; i < siz; i += 4) 
		{
			cout << dechex(i) << " ";
		for (int j = i; j < min(siz, i + 4); ++j) 
		{
			cout << dechex(machinecodes[j]) << " ";
		}
		cout << "\n";
		}
	}
	else if(type==5)
	{
		cout << "Memory dump after execution" << endl;
		for (int i = 0; i < siz; i += 4) 
		{
		cout << dechex(i) << " ";
		for (int j = i; j < min(siz, i + 4); ++j) 
		{
			cout << dechex(mem[j]) << " ";
		}
		cout << "\n";
		}
	}
	else if(type==6)
	{
		for(int i=0; i<read1.size(); i++)
		{
			cout << "Read memory[" << dechex(read1[i].add) << "] found " << dechex(read1[i].value) <<"\n";
		}
	}
	else if(type ==7)
	{
		for(int i=0; i<write1.size(); i++)
		{
			//cout<<"Wrote memory "<<dechex(write1[i].add)<<" was "<<dechex(write1[i].previous)<<"\n";
			cout << "Wrote memory[" <<dechex(write1[i].add) << "] was " << dechex(write1[i].previous) << " now " << dechex(write1[i].present)<<"\n";
		}
	}
	else
		break;
	}
	
	return 0;
}

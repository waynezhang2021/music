#pragma once
#include<bits/stdc++.h>
#include<pthread.h>
#include<string.h>
#include<csignal>
#include<cstdlib>
#include<vector>
#include<bitset>
#include<string>
#include<cstdio>
#include<queue>
#include<stack>
using namespace std;
void info(int argc,char** argv)
{
	if(argc==1)
	{
		printf("MIDI to dynamically computed PCM(pulse code modulation) C++ code tool.\nusage:%s <csv_file> <volume> <transpose_ratio>",argv[0]);
		exit(0);
	}
	if(argc==4)
		return;
	if(argc>=4||argc==2||argc==3)
	{
		printf("invalid number of parameters(should be 3)");
		exit(0);
	}
}
double process_note(string note)
{
	static int tbl[]= {0,2,4,5,7,9,11};
	double base=261.6255653005986346778499935234;
	double ratio=1.0594630943592952645618252949463;
	if(note[0]>='C'&&note[0]<='G')
		base*=pow(ratio,tbl[note[0]-'C']);
	else
		base*=pow(ratio,tbl[((note[0]=='A')?5:6)]);
	if(note[1]=='#')
	{
		base*=ratio;
		base*=pow(2,atoi(note.substr(2).c_str()));
		return base;
	}
	else
	{
		base*=pow(2,atoi(note.substr(1).c_str()));
		return base;
	}
//pure inotation:
//	static map<string,double> tbl={{"C",1},{"C#",16.0/15},{"D",9.0/8},{"D#",6.0/5},{"E",5.0/4},{"E#",4.0/3},{"F",4.0/3},{"F#",45.0/32},{"G",3.0/2},{"G#",8.0/5},{"A",5.0/3},{"A#",16.0/9},{"B",15.0/8},{"B#",2}};
//	if(note[1]=='#')
//		return 261.6255653005986346778499935233*pow(2,stoi(note.substr(2)))*tbl[note.substr(0,2)];
//	else
//		return 261.6255653005986346778499935233*pow(2,stoi(note.substr(1)))*tbl[note.substr(0,1)];
}
typedef struct
{
	string note;
	float start;
	float end;
	int velocity;
} midi_note;
void process_midi(string file,int volume,double transpose)
{
	if(file.substr(file.length()-4,4)!=".csv")
	{
		printf("the specified file is not a CSV file");
		exit(0);
	}
	ifstream in(file);
	FILE *out=fopen((file.substr(0,file.length()-4)+".cpp").c_str(),"w");
	if(!in)
	{
		printf("failed to open input file");
		exit(0);
	}
	if(!out)
	{
		printf("failed to open output file");
		exit(0);
	}
	string useless_str;
	float useless_flt;
	in>>useless_str>>useless_str>>useless_str>>useless_str>>useless_str>>useless_str;
	float music_length;
	while(in>>useless_str>>useless_str>>useless_flt>>music_length>>useless_flt>>useless_flt);
	in.clear();
	in.seekg(0);
	printf("detected music length:%fs\n",music_length);
	string note;
	float start,end;
	double freq;
	int track,velocity;
	int data_size=0,bufsize=0;;
	printf("process notes...");
	vector<midi_note> v[16];
	in>>useless_str>>useless_str>>useless_str>>useless_str>>useless_str>>useless_str;
	fprintf(out,"#include<windows.h>\n#include<math.h>\n#include<cstdio>\nusing namespace std;\ndouble pi=3.1415926535897932384626433832795;\n");
	while(in>>track>>note>>start>>end>>useless_flt>>velocity)
	{
		printf("%d ",track);
		v[track].push_back(midi_note{note,start,end,velocity});
		bufsize=max(bufsize,int(end*44100));
	}
	for(int i=0;i<16;i++)
	{
		fprintf(out,"float data_track_%d[]={",i);
		data_size=0;
		for(midi_note m:v[i])
		{
			freq=process_note(m.note);
			fprintf(out,"%lf,",freq*transpose);
			fprintf(out,"%d,",int(m.start*44100));
			fprintf(out,"%d,",int(m.end*44100));
			fprintf(out,"%lf,",m.velocity/127.0);
			data_size++;
		}
		fprintf(out,"};\nint data_size_track_%d=%d;\n",i,data_size);
	}
	fprintf(out,"int bufsize=%d;\n",bufsize);
	in.close();
	printf("done\n");
	printf("output program...");
	fclose(out);
	printf("done");
	return;
}
int main(int argc,char** argv)
{
	info(argc,argv);
	process_midi(argv[1],atoi(argv[2]),stod(argv[3]));
}

// ExperDataConversion.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void removeUnused(string fName);
void getData(ifstream& f);

int _tmain(int argc, wchar_t **argv)
{
	try {
		ifstream f;
		string openErr = "File with experiment result cannot be open";
		if (argc == 1)
			f.open("GenerateStatesEx.txt");
		else {
			f.open(argv[1]);
		}
		if (f.fail()) throw openErr;
		getData(f);
		removeUnused("jumpState.dat");
		removeUnused("zeroState.dat");
		removeUnused("jumpTime.dat");
		removeUnused("zeroTime.dat");
		removeUnused("stateTime.dat");
		f.close();
	}
	catch(const string msg){
		cout << msg << endl;
		exit(EXIT_FAILURE);
	}
	return 0;
}

void removeUnused(string fName){
	ifstream prevFile(fName);
	ofstream nextFile("next.txt");
	while (!prevFile.eof()){
		string s; 
		getline(prevFile, s);
		float val1, val2;
		istringstream iss(s);
		iss >> val1;
		iss >> val2;
		if (!iss.fail())
			nextFile << val1 << " " << val2 << endl; 
	}
	remove(fName.c_str());
	rename("next.txt",fName.c_str());
}

void getData(ifstream& f){
	try{
		ofstream fJS("jumpState.dat");
		ofstream fZS("zeroState.dat");
		ofstream fJT("jumpTime.dat");
		ofstream fZT("zeroTime.dat");
		ofstream fST("stateTime.dat");
		ofstream fZF("zeroFailure.dat");
		string formatError = "Wrong format at line ";
		vector <int> failCounts, zeroCounts;
		failCounts.resize(50); zeroCounts.resize(50);
		int line = 0;
		while (!f.eof()){
			string s;
			getline(f,s);
			++line;
			string toFind = "_j-";
			int found = s.find(toFind);
			if (found == string::npos)
				throw formatError + to_string((long long)line);
			s.erase(0,found + toFind.size());
			int j = -1;
			istringstream iss(s);
			iss >> j;
			if (iss.fail())
				throw formatError + to_string((long long)line);
			if (j < 1) throw "Wrong j value at line " + to_string((long long)line);
			fJS << j << " ";
			fJT << j << " ";
			getline(f,s);
			++line;
			toFind = "Zero count: ";
			found = s.find(toFind);
			if (found != 0)
				throw formatError + to_string((long long)line);
			s.erase(0, toFind.size());
			iss.clear();
			iss.str(s);
			int zeroCount = -1;
			iss >> zeroCount;
			if (iss.fail())
				throw formatError + to_string((long long)line);
			if (zeroCount < 1) throw "Wrong zero count value at line " + to_string((long long)line);
			fZS << zeroCount << " ";
			fZT << zeroCount << " ";

			zeroCounts[zeroCount-1]++;

			getline(f,s);
			++line;
			if (s == "SetFullPackageStates() was terminated, time was more than 10 s") {
				failCounts[zeroCount-1]++;
				continue;
			}
			
			toFind = "Time of set full states ";
			found = s.find(toFind);
			if (found != 0)
				throw formatError + to_string((long long)line);
			s.erase(0, toFind.size());
			iss.clear();
			iss.str(s);
			float time = -1;
			iss >> time;
			if (iss.fail())
				throw formatError + to_string((long long)line);
			if (time == -1) throw "Wrong time value at line " + to_string((long long)line);
			fJT << time << endl;
			fZT << time << endl;

			getline(f,s);
			++line;
			iss.clear();
			iss.str(s);
			float states = -1;
			iss >> states;
			if (iss.fail())
				throw formatError + to_string((long long)line);
			if (time == -1) throw "Wrong states value at line " + to_string((long long)line);
			fZS << states << endl;
			fJS << states << endl;
			fST << states << " " << time << endl;

		}
		for (int i = 0; i < failCounts.size(); i++)
			if (zeroCounts[i]!=0) fZF << i+1 << " " << (float)failCounts[i]/(float)zeroCounts[i] << endl;
		fJS.close();
		fZS.close();
		fJT.close();
		fZT.close();
		fST.close();
		fZF.close();
	}
	catch(const string msg){
		cout << msg << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
}
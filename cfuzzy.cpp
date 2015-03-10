/*
A Simple Fuzzy logic inference system for Threat evaluation
Author: Ngo Dinh Nguyen
Contacts: nguyenngodinh@outlook.com | nguyennd56coltech@gmail.com
==================================================================
How to use?
0. Environment: windows os
1. compile source
=== g++ cfuzzy.cpp -o cfuzzy
2. Resource requirement:
=== init.txt: define the fuzzy set (etc. speed-low, speed-medium, . . .)
====== with 1st line as define number of "member" which is as variable
====== the next line define the fuzzy sets of each member with informations: Interval, Middle (max membership grade), Type, Name
=== rules.txt: define fuzzy if-then rules as form with two inputs in left side, and one output in the right side
====== the rule be defined as index of fuzzy set be defined in init.txt
=== data.txt: where each line is the set of values for input params
=== results.txt: the file store result as threat value corresponding with value input
3. Run
=== cfuzzy.exe
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <cstring>
#include <fstream>
using namespace std;

class CFuzzyFunction
{
protected :
	double dLeft, dRight;
	char   cType;
	char*  sName;
public:
	CFuzzyFunction(){};
	virtual ~CFuzzyFunction(){ delete [] sName; sName = NULL;}
	virtual void setInterval(double l, double r){
		dLeft = l; 
		dRight = r;
	}
	virtual void setMiddle(double dL = 0, double dR = 0) = 0;
	virtual void setType(char c){cType = c;}
	virtual void setName(const char* s){ 
	  sName = new char[strlen(s)+1];
	  strcpy(sName, s);
	}
	bool isDotInInterval(double t){
		if((t >= dLeft) && (t <= dRight)) 
			return true; 
		return false;
	}
	char getType( void)const{ 
		return cType;	
	}
    void getName() const{
		cout << sName << endl;
	}
	virtual double getValue(double t) = 0;
};

class CTriangle : public CFuzzyFunction
{
private: 
	double dMiddle;
public:
	void setMiddle(double dL, double dR){
		dMiddle = dL;
	}
	double getMiddle(){
		return dMiddle;
	}
	double getValue(double t){
		if(t <= dLeft)
			return 0;
		else if(t < dMiddle)
			return (t-dLeft)/(dMiddle-dLeft);
		else if(t==dMiddle)
			return 1.0;
		else if(t<dRight)
		    return (dRight-t)/(dRight-dMiddle);
		return 0;
	}
};

struct Member
{
	CTriangle* fuzzySet;
	int numberOfValue;
};
struct Rule
{
	int leftSide[2][2];
	int rightSide[2];
	int fuzzyOperator;
};
struct DataInput
{
	double input[4];
};
struct Member * members;
struct Rule * rules;
struct DataInput * dataInput;
double * output;
int defineFuzzySet(const char * );
int defineRules(const char * );
int readData(const char * , int);
double ruleEvaluation(struct Rule, struct DataInput );
double defuzzyfication(struct Rule * , int , struct DataInput);


int main(void)
{
	
	int numberOfMember = defineFuzzySet("init.txt");
	int numberOfRule = defineRules("rules.txt");
	int numberOfData = readData("data.txt", numberOfRule);
	output = new double[numberOfData];
	FILE * resultFile = fopen("results.txt", "w");
	fprintf(resultFile, "Threat result: \n");
	double sum = 0;
	for(int i=0; i< numberOfData; i++)
	{
		output[i] = defuzzyfication(rules, numberOfRule, dataInput[i]);
		sum += output[i];
		fprintf(resultFile, "%d. %f\n", i, output[i]);
	}
	cout << "average : " << sum/numberOfData;
	fprintf(resultFile, "average: %f\n", sum/numberOfData);
	fclose(resultFile);
	return EXIT_SUCCESS;
}

int defineFuzzySet(const char * fileName)
{
	FILE * file = fopen(fileName, "r");
	int numberOfMember;
	fscanf(file, "Number of member: %d\n", &numberOfMember);
	members = new struct Member[numberOfMember];
	
	/*
	Initialize member function
	*/
	for(int i = 0; i < numberOfMember; i++)
	{
		int numberOfFuzzySet;
		double dtemp1, dtemp2;
		char ctemp;
		char stemp[50];
		fscanf(file, "Number of fuzzySet %d\n", &numberOfFuzzySet);
		members[i].numberOfValue = numberOfFuzzySet;
		members[i].fuzzySet = new CTriangle[numberOfFuzzySet];
		
		for(int j = 0; j < numberOfFuzzySet; j++)
		{
			fscanf(file, "Interval %lf %lf\n", &dtemp1, &dtemp2); 
			members[i].fuzzySet[j].setInterval(dtemp1, dtemp2); 
			fscanf(file, "Middle %lf %lf\n", &dtemp1, &dtemp2); 
			members[i].fuzzySet[j].setMiddle(dtemp1, dtemp2);
			fscanf(file, "Type %c\n", &ctemp);
			members[i].fuzzySet[j].setType(ctemp);
			fscanf(file, "Name %s\n", &stemp);
			members[i].fuzzySet[j].setName(stemp);
		}
		
	}
	fclose(file);
	return numberOfMember;
}
int defineRules(const char * fileName)
{
	/*
	Initialize rules
	*/
	int numberOfRule;
	FILE * ruleFile  = fopen(fileName, "r");
	fscanf(ruleFile, "Rules: %d\n", &numberOfRule);
	rules = new struct Rule[numberOfRule];
	for(int i = 0; i< numberOfRule; i++)
	{
		int itemp1, itemp2;
		fscanf(ruleFile, "LeftSide1 %d %d\n", &itemp1, &itemp2);
		rules[i].leftSide[0][0] = itemp1; rules[i].leftSide[0][1] = itemp2;
		fscanf(ruleFile, "LeftSide2 %d %d\n", &itemp1, &itemp2); 
		rules[i].leftSide[1][0] = itemp1; rules[i].leftSide[1][1] = itemp2;
		fscanf(ruleFile, "Operator %d\n", &itemp1); 
		rules[i].fuzzyOperator = itemp1;
		fscanf(ruleFile, "RightSide %d %d\n", &itemp1, &itemp2); 
		rules[i].rightSide[0] = itemp1; rules[i].rightSide[1] = itemp2;
	}
	fclose(ruleFile);
	return numberOfRule;
}
int readData(const char * fileName, int numberOfRule)
{
	/*
	Read input file 
	*/
	
	FILE * input = fopen(fileName, "r");
	int numberOfData;
	fscanf(input, "Number of data: %d\n", &numberOfData);
	cout << "number of data: " << numberOfData << endl;
	dataInput = new DataInput[numberOfData];
	fscanf(input, "Speed		Altitude	CPA		TBH");
	for(int i=0; i<numberOfData; i++)
	{
		fscanf(input, "%lf	%lf	%lf	%lf\n", &dataInput[i].input[0], 
				&dataInput[i].input[1], &dataInput[i].input[2], &dataInput[i].input[3]);
	}
	
	return numberOfData;
}
double ruleEvaluation(struct Rule rule, struct DataInput dataInput)
{
	int memberIndex, fuzzySetIndex;
	double leftValue1, leftValue2;
	
	memberIndex = rule.leftSide[0][0];
	fuzzySetIndex = rule.leftSide[0][0];
	leftValue1 = members[memberIndex].fuzzySet[fuzzySetIndex].getValue(dataInput.input[memberIndex]);
	
	memberIndex = rule.leftSide[1][0];
	fuzzySetIndex = rule.leftSide[1][1];
	leftValue2 = members[memberIndex].fuzzySet[fuzzySetIndex].getValue(dataInput.input[memberIndex]);
	
	if(rule.fuzzyOperator == 0)
		return fmin(leftValue1, leftValue2);
	else return fmax(leftValue1, leftValue2);
}
double defuzzyfication(struct Rule * rule, int ruleNumber, struct DataInput dataInput)
{
	double dtemp1 = 0;
	double dtemp2 = 0;
	for(int i=0; i< ruleNumber; i++)
	{
		double temp = ruleEvaluation(rule[i], dataInput);
		double output = members[rules[i].rightSide[0]].fuzzySet[rules[i].rightSide[1]].getMiddle();
		dtemp1 += temp;
		dtemp2 += temp * output;
	}
	if(dtemp1 == 0)
		return 0;
	return dtemp2/dtemp1;
}
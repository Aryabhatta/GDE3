#ifndef GDEALGORITHM_H_
#define GDEALGORITHM_H_

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>

#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "ISearchAlgorithm.h"
#include "../datamodel/Variant.h"
using namespace std;

class Gde3Algorithm: public ISearchAlgorithm
{
		
	double F,CR;
	
public:
	VariantSpace* variantSpace;
	
	vector<TuningPoint*> tuningPointVec; // additional data structure to maintain Tuning point ptrs
	bool searchFinished;
	
	std::size_t populationSize;
	vector<Variant> population;	// instead of scenario's here handled as population
	set<std::string> populElem;	
			
	double optimalObjVal; 				 // can be replaced by vector for MO functions
	
	map<int,int> parentChildMap;
	set<int> tobeDropped;
	
	string logString; // keeps track of all activities, helps to debug
	
public:
	Gde3Algorithm();
	virtual ~Gde3Algorithm();
		
	void addSearchSpace(VariantSpace*);
	void createScenarios();
	void doSearch() {}
	int getOptimum();	
	bool isSearchFinished() const;
	void setSearchFinished(bool searchFinished);
				
	TuningPoint * createTuningPoint(long id, string tuningActionName, tunableType parameterType, int minRange,
					  					int maxRange);
	void cleanupPopulation();
};

#endif /*GDEALGORITHM_H_*/

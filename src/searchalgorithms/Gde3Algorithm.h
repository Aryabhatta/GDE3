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
	SearchSpace * searchSpace;

	std::size_t populationSize;
	vector<Variant> population;			// instead of scenario's here handled as population
	vector<Variant> recentPopulation; 	// keeps track of last population
	set<std::string> populElem;			// makes sure the variants are unique
			
	double optimalObjVal; 				 // can be replaced by vector for MO functions
	string optimalVariant;
	
	map<int,int> parentChildMap;
	set<int> tobeDropped;
	
	string logString; // keeps track of all activities, helps to debug
	
	int samePopulationVector;
	
public:
	Gde3Algorithm();
	virtual ~Gde3Algorithm();
	
	
	// Inhertied and not used
	void initialize(void) {}
	void clear(void) {}
	map<int, double> getSearchPath(void) {}	
	void terminate(void) {}
	void finalize(void) {}
	bool searchFinished(void) {return false;}
	
	void addSearchSpace(SearchSpace*);
	void createScenarios();
	int getOptimum();	
	
	bool searchFinished(map< string,vector<double> > & evalVec);// { return false;}
	
	TuningParameter * createTuningPoint(long id, string tuningActionName, tPlugin parameterType, int minRange,
					  					int maxRange);
	
	bool checkFeasible(Variant & varEval);
	int compareVariants(Variant & parent, Variant & child, map< string,vector<double> > & evalVec);
	void cleanupPopulation( map< string,vector<double> > & evalVec );	
};

#endif /*GDEALGORITHM_H_*/

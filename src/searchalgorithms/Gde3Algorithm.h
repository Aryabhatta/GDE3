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
//	VariantSpace* variantSpace;
	SearchSpace * searchSpace;
	
	vector<TuningPoint*> tuningPointVec; // additional data structure to maintain Tuning point ptrs
	bool searchFinishedVar;
	
	std::size_t populationSize;
	vector<Variant> population;	// instead of scenario's here handled as population
	vector<Variant> recentPopulation; // keeps track of last population
	set<std::string> populElem;	
			
	double optimalObjVal; 				 // can be replaced by vector for MO functions
	
	map<int,int> parentChildMap;
	set<int> tobeDropped;
	
	string logString; // keeps track of all activities, helps to debug
		
	int samePopulationVector = 0;
	
public:
	Gde3Algorithm();
	virtual ~Gde3Algorithm();
	
	// TODO compare with new ISearchAlgorithm interface and remove functions not necessary
	
	// Inhertied and not used
	void initialize(void) {}
	void clear(void) {}
	map<int, double> getSearchPath(void) {}
	bool searchFinished(void) { return false;}
	void terminate(void) {}
	void finalize(void) {}
	
	void addSearchSpace(SearchSpace*);
	void createScenarios();
	int getOptimum();	
	
	// TODO Only one search finished method stays as per ISearchAlgorithm
	bool isSearchFinished() const;
	void setSearchFinished(bool searchFinished);
	void checkSearchFinished( map< string,vector<double> > & evalVec);
				
	TuningPoint * createTuningPoint(long id, string tuningActionName, tunableType parameterType, int minRange,
					  					int maxRange);
	
	bool checkFeasible(Variant & varEval);
	int compareVariants(Variant & parent, Variant & child, map< string,vector<double> > & evalVec);
	void cleanupPopulation( map< string,vector<double> > & evalVec );	
};

#endif /*GDEALGORITHM_H_*/

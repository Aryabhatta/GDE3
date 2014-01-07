#ifndef GDEALGORITHM_H_
#define GDEALGORITHM_H_

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "ISearchAlgorithm.h"
#include "../datamodel/Variant.h"
using namespace std;

class GdeAlgorithm: public ISearchAlgorithm
{
	VariantSpace* variantSpace;
	bool searchFinished;
	
	int populationSize;
	vector<Variant> population;
	set<std::string> populElem;
	
	vector<TuningPoint*> tuningPointVec; // additional data structure to maintain Tuning point ptrs		
	double optimalObjVal; // can be replaced by vector for MO functions
	
	map<int,int> parentChildMap;
	vector<int> tobeDropped;
	
	double F,CR;
	
	bool checkFeasible(Variant & varEval);
public:
	GdeAlgorithm();
	virtual ~GdeAlgorithm();
	
	void addSearchSpace(VariantSpace*);
	void createScenarios();
	void doSearch();
	int getOptimum();	
	bool isSearchFinished() const;
	void setSearchFinished(bool searchFinished);
			
	VariantSpace * initialize();
	void evaluate(int parentIdx, int childIdx);
	double evaluate(Variant & member); // Function to print results, can be removed
	
	TuningPoint * createTuningPoint(long id, string tuningActionName, tunableType parameterType, int minRange,
					  					int maxRange);
};

#endif /*GDEALGORITHM_H_*/

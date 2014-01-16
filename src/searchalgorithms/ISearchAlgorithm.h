#ifndef ISEARCHALGORITHM_H_
#define ISEARCHALGORITHM_H_

#include <iostream>
#include "../datamodel/VariantSpace.h"
using namespace std;

class ISearchAlgorithm
{
public:
	ISearchAlgorithm() {}
	virtual ~ISearchAlgorithm() {}
	virtual void addSearchSpace(VariantSpace*) = 0;	
	virtual void createScenarios() = 0;
	virtual void doSearch() = 0;						// return type changed from queue<ScenarioDescription*>*	
	virtual int getOptimum() = 0;						// TBD: what optimum is in our context
	virtual bool isSearchFinished() const = 0;
	virtual void setSearchFinished(bool searchFinished) = 0;
	
//	virtual queue<ScenarioDescription*>* doSearch() = 0;
//	virtual map<int, MetaProperty* > getSearchPath() = 0;
//	virtual int getObjective() const = 0;
//	virtual void setObjective(int objective) = 0;
//	virtual void processResults(list<MetaProperty> metaProps) = 0;
};

#endif /*ISEARCHALGORITHM_H_*/

#ifndef GDE3SEARCH_H_
#define GDE3SEARCH_H_

// Autotune includes
#include "AutotuneSearchAlgorithm.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>

#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

bool timerExceeded = false;
void timerFunction( double curTime );

class GDE3Search: public ISearchAlgorithm {

    double F,CR;
    SearchSpace * searchSpace;
    int GenerationNo;
    int noAttempts; // attempts to create a variant
//    bool timeExceeded; // to use later with Register timer functionality
    ScenarioPoolSet *pool_set;

    bool checkFeasible(Variant & variant);
public:
    GDE3Search();
    virtual ~GDE3Search();

    std::size_t populationSize;
    vector<Variant *> population;         // instead of scenario's here handled as population
    vector<Variant *> recentPopulation;   // keeps track of last population
    set<std::string> populElem;         // makes sure the variants are unique
    map<string,int> variantScenarioMap;
            
    double optimalObjVal;                // can be replaced by vector for MO functions
    string optimalVariant;
    
    map<int,int> parentChildMap;
    set<int> tobeDropped;
    
    string logString; // keeps track of all activities, helps to debug
    
    int samePopulationVector;

    void initialize(DriverContext *context, ScenarioPoolSet *pool_set);
    void clear();
    void addSearchSpace(SearchSpace*);
    void createScenarios();
    int getOptimum();
    map<int, double > getSearchPath();
    bool searchFinished();
    void terminate();
    void finalize();

    int compareVariants(Variant & parent, Variant & child);
    void cleanupPopulation();   
};

#endif 

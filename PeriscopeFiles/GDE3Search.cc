#include "GDE3Search.h"

GDE3Search::GDE3Search() : ISearchAlgorithm(){
    search_steps = 0;
    GenerationNo = 0;

    // Minimization problem
    optimalObjVal = 100000.0;
    
    // Set initial population size
    populationSize = 5;    
    samePopulationVector = 0;

    noAttempts=0;
    CR = 0.5;
    F = 0.5;
}

ISearchAlgorithm* getSearchAlgorithmInstance(void) {
  return new GDE3Search();
}

GDE3Search::~GDE3Search() { 
    delete searchSpace;
}

bool GDE3Search::checkFeasible(Variant & variant) {
	
  vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
  map<TuningParameter*, int > vectorMap = variant.getValue(); 
  for(size_t i=0; i< tuningParVec.size(); i++) {
	
	if(vectorMap[tuningParVec[i]] < tuningParVec[i]->getRangeFrom()) {
		return false;
	}

	if(vectorMap[tuningParVec[i]] > tuningParVec[i]->getRangeTo()) {
		return false;
	}

	// need to find if it is in rules with stepsi
	bool valid = false;
	for(size_t iter = tuningParVec[i]->getRangeFrom(); iter <= tuningParVec[i]->getRangeTo(); iter += tuningParVec[i]->getRangeStep()){
		if(iter == vectorMap[tuningParVec[i]]) {
			valid = true;
			break;
		}
	}
	if(!valid){
		return false;
	}	
  }
  return true;
}

string toString( Variant & variant, vector<TuningParameter*> & tuningParVec ) {
    map<TuningParameter*, int > vectorMap = variant.getValue(); 
    string newString;
    
    for(size_t i=0; i< tuningParVec.size(); i++) {
        stringstream newS;
        newS << vectorMap[tuningParVec[i]];
        newString.append(newS.str());
        newString.append(",");      
    }
        
    return newString;
}

void timerFunction(double curTime) {
    timerExceeded = true;
    cout << "### Timer exceed signal received from context" << endl;
}

void GDE3Search::initialize(DriverContext *context, ScenarioPoolSet *pool_set){
    psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to initialize()\n");
    this->pool_set = pool_set;

    void (*funcPtr) (double);
    funcPtr = &timerFunction;
    context->register_timer(funcPtr,10.0);
}

bool GDE3Search::searchFinished(){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to searchFinished()\n");
  printf("\n### In GDE3 Searchfinished !\n");

  bool searchFinish = false;
  vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
	
  // evaluate all configurations in population	
  for(std::size_t i=0; i<population.size(); i++)
  {	
	if(parentChildMap.count(i) > 0 ) { // parent
		int parent = i;
		int child = parentChildMap[i];
		
		// send for evaluation
		int resultComp = compareVariants((*population[parent]), (*population[child]));
		
		if( resultComp == 0) { // both dominant
			tobeDropped.insert(child); // TODO: Remove after implemeting crowding distance logic, for multiple objective function
		} else if(resultComp < 0) { //Parent Dominant
			tobeDropped.insert(child);
		} else if( resultComp > 0) { // Child Dominant
			tobeDropped.insert(parent);
		}				
	}
  }
	
  printf("### After adding to tobeDropped list !\n");
  cleanupPopulation();
  printf("### After cleanupPopulation !\n");
	
  // if the population has more elements than population size, clear depending on Crowding distance
  if(population.size() > populationSize) {
	bool resizingDone = false;
	for(std::size_t i=0; i < population.size(); i++) {
		for(std::size_t j=i+1; j < population.size(); j++) {
			
			// send for evaluation
			int resultComp = compareVariants((*population[i]), (*population[j]));
			
			if( resultComp == 0) { // both non-dominant
				// do nothing, cannot lose non-dominant vectors
			} else if(resultComp < 0) { //Parent Dominant
				tobeDropped.insert(j);
			} else if( resultComp > 0) { // Child Dominant
				tobeDropped.insert(i);
			}				
			
			if(population.size()-tobeDropped.size() == populationSize) {
				resizingDone = true;
				break;
			}											
		}				
		if(resizingDone) {
			break;
		}
	}
	
	if(!resizingDone) { // all non-dominant vector, need to clean by crowding distance
						// TODO:Logic for crowding distance
	}
	
	// clear elements from population vector
	cleanupPopulation();
    printf("### After second cleanupPopulation !!!\n");
  }
	
  // Refill the unique set again, so that there are no repetition of configurations	
/*  for( vector<Variant*>::iterator iter = population.begin(); iter != population.end(); ++iter) {		
	map<TuningParameter*,int> vectorMap = (*iter)->getValue();
	string uniqueElem = toString((*(*iter)),tuningParVec);
	populElem.insert(uniqueElem);		
  }
  printf("### After refiling unique set of populElem !\n");*/
	
  // Print Population after cleaning
  logString.append("\nCleaned Population: \n { ");
  for(vector<Variant*>::iterator iter= population.begin();iter!= population.end(); ++iter) {
	logString.append("(");
	logString.append(toString((*(*iter)), tuningParVec));
	logString.append(") , ");
  }
  logString.append("}\n");
	
  // Priting evaluation of this generation, later this code can be removed
  logString.append("Objective values after generation:\n {" );
  for(vector<Variant*>::iterator iter= population.begin();iter!= population.end(); ++iter) {
	
  vector<double> objVal;

  objVal.push_back(objectiveFunction(variantScenarioMap[toString((*(*iter)),tuningParVec)], pool_set->srp));

  logString.append("(");
  for(size_t i=0; i<objVal.size(); i++) {
	stringstream ss1;
	ss1 << objVal[i];
	logString.append( ss1.str() + ",");
  }
  logString.append(")");
  }

  logString.append("}\n");
  
  printf("### Comparing Population sizes now !\n");
	
  if(recentPopulation.empty()) { // first time
	vector<Variant*>::iterator iter;
	for(iter=population.begin(); iter != population.end(); ++iter) {
		recentPopulation.push_back(*iter);
	}			
  }
	
  bool generationEqual = true;
	
  // compare two population vectors
  if(population.size() == recentPopulation.size()) {
	for(std::size_t i=0; i< recentPopulation.size(); i++) {
		if( toString((*population[i]),tuningParVec) != toString((*recentPopulation[i]),tuningParVec) ){
			generationEqual = false;
			break;
		}
	}
	
	if(generationEqual) {
		samePopulationVector++;
	} else {
        samePopulationVector = 1;
   }
  }
	
  // replace replacePopulation by newest one
  recentPopulation.clear();
  vector<Variant*>::iterator iter;
  for(iter=population.begin(); iter != population.end(); ++iter) {
	recentPopulation.push_back(*iter);
  }
  
  if(samePopulationVector == 3 || GenerationNo == 10  || noAttempts >= 10000 || timerExceeded ) {    
	searchFinish = true;    
    cout << logString << endl;
    cout << "### Application Completed after " << GenerationNo << " generations !!!" << endl;
    if(timerExceeded){
        cout << "### TERMINATION due to TIMEOUT" << endl;
    }
  }
 
  this->getOptimum();
  printf("### samePopulationVector value: %d\n",samePopulationVector);
  
  return searchFinish;
//    return true;// debugging purpose
}

int GDE3Search::getOptimum() {
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getOptimum()\n");

    //printing optimal value here
    stringstream ssObjVal;
    ssObjVal << optimalObjVal;

    cout <<"### Optimal Value till now: " << ssObjVal.str()  << endl;
    cout <<"### Optimal Variant till now: " << this->optimalVariant << endl;
    
    return 0;
}

void GDE3Search::clear() {
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to clear()\n");
}

map<int, double > GDE3Search::getSearchPath() {
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getSearchPath()\n");

}

void GDE3Search::addSearchSpace(SearchSpace* searchSpace) {
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to addSearchSpace()\n");
  this->searchSpace = searchSpace;
}

void GDE3Search::createScenarios() {
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to createScenarios()\n");
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneAll), "GDE3 Search: Starting the search\n");
  
    GenerationNo++;
    stringstream ss;
    ss << GenerationNo;
    logString.append("\n\n Generation No: ");
    logString.append(ss.str());
    logString.append("\nPopulation: \n { ");

    // initialize no of attempts to create
    noAttempts=0;
    
    // initialize random seed
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec);

    int membertoCreate = populationSize;
    
    vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters(); 
    printf("\n### Accessed Tuning ParVec, Size = %d\n", tuningParVec.size());        
    if(population.empty()) { // first time in create Scenarios      
        while(true) {
            
            Variant * newVariant = new Variant();
            map<TuningParameter*,int> newMap;
            
            for(std::size_t i=0; i<tuningParVec.size(); i++ ) {
//                int value = rand()%(tuningParVec[i]->getRangeTo()-tuningParVec[i]->getRangeFrom()) + tuningParVec[i]->getRangeFrom();
                double randGen = (double)rand()/(double)(RAND_MAX);
                double valDbl = randGen * (tuningParVec[i]->getRangeTo()-tuningParVec[i]->getRangeFrom()) + tuningParVec[i]->getRangeFrom();
//                printf("## TuningPar - RangeFrom: %d, Rangeto: %d, RangeStep: %d\n", tuningParVec[i]->getRangeFrom(), tuningParVec[i]->getRangeTo(), tuningParVec[i]->getRangeStep());
//                cout << "## Rand Generated: " << randGen << endl;
//                cout << "## Val generated: " << valDbl << endl;

		int normFrom = tuningParVec[i]->getRangeFrom() / tuningParVec[i]->getRangeFrom(); // should be 1 always
		int normTo = tuningParVec[i]->getRangeTo() / tuningParVec[i]->getRangeFrom();

		// normalization if the range does not starts from 1
		if( tuningParVec[i]->getRangeFrom() != 1) {
			valDbl = valDbl / tuningParVec[i]->getRangeFrom();
		}

                // round it to nearest valid point
                int value = round(valDbl);

		// denormalizing the tuning parameter if it has been normalizes before
		if( tuningParVec[i]->getRangeFrom() != 1) {
			value = value * tuningParVec[i]->getRangeFrom();
		}

//                cout << "## Val rounded to " << value << endl;
                newMap[tuningParVec[i]] = value;                        
            }
            
	    noAttempts++;
	    if(noAttempts==10000) { // dont entertain much
		// abort in between
	        cout << "### No of attempts at generating population exceeded !!!" << endl;
	        cout << "### Returning from createScenarios without pushing any scenarios !!!" << endl;
		return;
	    }

            newVariant->setValue(newMap);

	    // check if variant is feasible or not, if not then don't push
            if(!checkFeasible(*newVariant)) {
		continue;
	    }

            string uniqueConfig = toString(*newVariant, tuningParVec);
            
            if(populElem.count(uniqueConfig) == 1) { //Already in population
                continue;
            } 
            
            printf("### New variant with value = %s created\n",uniqueConfig.c_str());
            populElem.insert(uniqueConfig);
            population.push_back(newVariant); // TODO: replace by adding to scenario pool
            membertoCreate--;
            if(membertoCreate == 0) {
		cout << "\n### No of attempts INITIAL: " << noAttempts << endl;
		noAttempts = 0;
            	break;
            }
        }           
     }
    
    printf("### Created initial Population !\n");        
    
    // Generate new population according to GDE3 population generation, even works for first random iteration
    int popSize = population.size();    
    int idx = 0;
    
    while(idx < popSize)
    {
        // For every member of population
        Variant * newChildVariant = new Variant();
        map<TuningParameter*,int> newChildMap;
        
        // Choose parent
        map<TuningParameter*,int> parentMap = population[idx]->getValue();
        
        // Choose 3 random members from population
        int random[3] = {-1,-1,-1};
        int i=0;
        while( i < 3) {
            int newEntry = rand() % (population.size());
            if(newEntry != random[((i+1)%3)] && newEntry != random[((i-1)%3)]) { // ensuring no repeat
                random[i++] = newEntry;
            }
        }
        
        map<TuningParameter*,int> rOneMap = population[random[0]]->getValue();
        map<TuningParameter*,int> rTwoMap = population[random[1]]->getValue();
        map<TuningParameter*,int> rThreeMap = population[random[2]]->getValue();

        size_t randIndex = floor(rand()%tuningParVec.size()) + 1; // TODO: check correct execution for multiple
        
        // iterate over all TP of variant
        for(std::size_t j=0; j<tuningParVec.size(); j++) {	
            double randGen = (double)rand()/(double)(RAND_MAX);
            if( randGen < CR || j == randIndex ) {
                newChildMap[tuningParVec[j]] = rThreeMap[tuningParVec[j]] + F*(rOneMap[tuningParVec[j]] - rTwoMap[tuningParVec[j]]);
//              printf("\n ### Doing cross over: value = %d",newChildMap[tuningParVec[j]]);
//		cout << "### First: " << rThreeMap[tuningParVec[j]] << endl;
//		cout << "### Second: " << rOneMap[tuningParVec[j]] << endl;
//		cout << "### Third: " <<  rTwoMap[tuningParVec[j]] << endl;
            }
            else {
                newChildMap[tuningParVec[j]] = parentMap[tuningParVec[j]];
//                printf("\n ### Copying directly from parent:value = %d",newChildMap[tuningParVec[j]]);
            }               
        }
         
	noAttempts++; 
	if(noAttempts==10000) { // dont entertain much
	    // abort in between
	    cout << "### No of attempts at generating population exceeded !!!" << endl;
	    cout << "### Returning from createScenarios without pushing any scenarios !!!" << endl;
	    return;
	}
        
        newChildVariant->setValue(newChildMap);

	// check if variant is feasible or not, if not then don't push
        if(!checkFeasible(*newChildVariant)) {
		continue;
	}

        // Need to make sure there are no repetitions in the population
        string uniqueConfig = toString(*newChildVariant,tuningParVec);
        if(populElem.count(uniqueConfig) == 1) { // config already present, redo
            continue;
        }
        
        idx++;      
        populElem.insert(uniqueConfig);
        population.push_back(newChildVariant);// TODO:replace by adding to scenario pool
        printf("### New variant with value = %s created\n",uniqueConfig.c_str());

        logString.append("(" + toString((*population[idx-1]),tuningParVec) + "),");
        logString.append("(" + toString(*newChildVariant,tuningParVec) + "),");      
        
        // Add reference in parent child map for evaluate function
        parentChildMap[idx-1] = population.size()-1;        
    }
    logString.append("}");
  
    cout << "\n### No of attempts: " << noAttempts << endl;
    psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneAll), "GDE3 Search: Adding variants to scenario pool.\n");
    
    // Adding the variants in population to scenario pool 
    list<TuningSpecification*> *ts = new list<TuningSpecification*> ();
    
    vector<Variant*>::iterator iter;
    for(iter=population.begin();iter!=population.end();++iter) {

    	Variant * variant = (*iter);

        // to make sure the variants are not evaluated twice
        if(variantScenarioMap.count(toString(*variant,tuningParVec)) != 1) {
        list<TuningSpecification*> *tsList = new list<TuningSpecification*> ();
          	
		list<Region*> *regions;
		regions = new list<Region*>;
		regions->push_back(searchSpace->getRegions()[0]);
		
		TuningSpecification* tuningSpec;
		tuningSpec = new TuningSpecification(variant, regions);
		VariantContext context = tuningSpec->getVariantContext();
		Variant *var= const_cast<Variant*>(tuningSpec->getVariant());
		TuningSpecification *temp= new TuningSpecification(var, context.context_union.region_list);
	    tsList->push_back(temp);

	    Scenario* scenario;		
		scenario = new Scenario(NULL, tsList, NULL);
		//scenario->print();
		
		//Insert into Map, to retrieve later
		variantScenarioMap[toString(*variant,tuningParVec)] = scenario->getID();
		pool_set->csp->push(scenario);		
        }
    }    
}

void GDE3Search::terminate(){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to terminate()\n");
}

void GDE3Search::finalize(){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to finalize()\n");
}

ISearchAlgorithm* getSearchInstance(void){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getSearchInstance()\n");

	return new GDE3Search();
}

int getSearchInterfaceVersionMajor(void){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getSearchInterfaceVersionMajor()\n");

	return 1;
}

int getSearchInterfaceVersionMinor(void){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getSearchInterfaceVersionMinor()\n");

	return 0;
}

string getSearchName(void){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getSearchName()\n");

	return "GDE3 Search";
}

string getSearchShortSummary(void){
  psc_dbgmsg(PSC_SELECTIVE_DEBUG_LEVEL(AutotuneSearch), "GDE3Search: call to getSearchShortSummary()\n");

	return "GDE3 search algorithm class.";
}

int GDE3Search::compareVariants(Variant & parent, Variant & child) {

    vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
    
    vector<double> parentMap; 
    parentMap.push_back(objectiveFunction(variantScenarioMap[toString(parent,tuningParVec)], pool_set->srp));

    vector<double> childMap;
    childMap.push_back(objectiveFunction(variantScenarioMap[toString(child,tuningParVec)], pool_set->srp));

    if(parentMap.size() != childMap.size()) {
        cout << "Fatal Erorr !" << endl;
        return -100;
    }
    
    // TODO: Temporary code, to remove
    // checking if its the optimal variant
    if( parentMap[0] < optimalObjVal ) {
        optimalObjVal = parentMap[0];
        this->optimalVariant = toString(parent, tuningParVec);
    }
    if( childMap[0] < optimalObjVal ) {
        optimalObjVal = childMap[0];
        this->optimalVariant = toString(child, tuningParVec);
    }

    // check for feasibility first, make sure it obeys all constraints
    if(!checkFeasible(parent)) {        
        // if not feasible, drop it from population
        return 1;
    }
    
    // check for feasibility first, make sure it obeys all constraints
    if(!checkFeasible(child)) {     
        // if not feasible, drop it from population
        return -1;
    }

    
    // compare for Parent dominance
    bool parentDom = true;
    for(size_t i=0; i<parentMap.size(); i++) {
        if(parentMap[i] > childMap[i]) { // 'Minimization' problem
            parentDom = false;
            break;
        }
    }
    
    if(parentDom) {
        return -1;
    }
    
    // compare for child dominance
    bool childDom = true;
    for(size_t i=0; i<parentMap.size(); i++) {
        if(parentMap[i] < childMap[i]) { // 'Minimization' problem
            childDom = false;
            break;
        }
    }
    
    if(childDom) {
        return 1;
    }
    
    return 0; // non dominance of both  
}

void GDE3Search::cleanupPopulation() {
    
    // End of iteration, cleanup mess
    vector<Variant*> popuNextGen;
    vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
    
    for(std::size_t i=0; i< population.size(); i++) {       
        if(tobeDropped.count(i) != 1 ) { //not found
            popuNextGen.push_back(population[i]);
        } else {
            // found in tobeDropped, remove from evalVec
            // evalVec.erase(toString(population[i],tuningParVec));
        }
    }
    
    population.clear();
    population = popuNextGen;
    parentChildMap.clear();
    tobeDropped.clear();
    //populElem.clear();
}

int getVersionMajor(void) {
  return 1;
}

int getVersionMinor(void) {
  return 0;
}

string getName(void) {
  return "GDE3 Search";
}

string getShortSummary(void) {
  return "Evolutionary algorithm that keeps best variants and discards other during course of generations.";
}

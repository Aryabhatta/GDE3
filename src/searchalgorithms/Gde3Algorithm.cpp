#include "Gde3Algorithm.h"

Gde3Algorithm::Gde3Algorithm(){

	// Minimization problem
	optimalObjVal = 100000.0;
	
	// Set initial population size
	populationSize = 10;	
	searchFinishedVar = false;
	
	CR = 0.5;
	F = 0.5;
}

Gde3Algorithm::~Gde3Algorithm() {
	// destroy all dynamically allocated memory
	delete searchSpace;
	
	vector<TuningParameter*>::iterator iter;
	for( iter = tuningPointVec.begin(); iter != tuningPointVec.end(); ++iter) {
		delete *iter;
	}
}

TuningParameter * Gde3Algorithm::createTuningPoint(long id, string tuningActionName, tPlugin pluginType, int minRange,
			  					int maxRange) {
	// Adding first parameter
	TuningParameter * tuningPointNew = new TuningParameter();
	tuningPointNew->setId(id);
	tuningPointNew->setName(tuningActionName);
	tuningPointNew->setPluginType(pluginType);
	
//	TuningPointRange tuningPointNewRange(minRange,maxRange); 
//	tuningPointNew->setTpRange(tuningPointNewRange);
	tuningPointNew->setRange(minRange,maxRange,1);
	
	// TODO: Currently, not specified anything as restriction, to change
	Restriction restriction;
	tuningPointNew->setRestriction(&restriction);
	
	if(true) {
	cout << "Tuning plugin create with id: " << id << " Name: " << tuningActionName << " Parameter type: "; 
	cout << pluginType << " TuningPointRange: " << minRange << " to " << maxRange << endl;
	}
	
	return tuningPointNew;
}

/***************************************************************************
 * Adds search space to local pointer to search space
 **************************************************************************/
void Gde3Algorithm::addSearchSpace( SearchSpace * space ) {
	this->searchSpace = space;
}

string toString( Variant & variant, vector<TuningParameter*> & TuningPointVec ) {
	map<TuningParameter*, int > vectorMap = variant.getValue();	
	string newString;
	
	for(size_t i=0; i< TuningPointVec.size(); i++) {
		stringstream newS;
		newS << vectorMap[TuningPointVec[i]];
		newString.append(newS.str());
		newString.append(",");		
	}
		
	return newString;
}


/***************************************************************************
 * For first time, creates random population
 * Creates new configurations
 **************************************************************************/
void Gde3Algorithm::createScenarios() {
	
	logString.append("\n\nGeneration Population: \n { ");
	
	// initialize random seed
	srand(time(NULL));
	int membertoCreate = populationSize;
			
	if(population.empty()) { // first time in create Scenarios		
		while(membertoCreate) {
			
			vector<int> tuningParameter;
			
			for(std::size_t i=0; i<tuningPointVec.size(); i++ ) {
				int value = rand()%populationSize + (-5);
				tuningParameter.push_back(value);
			}
			
			Variant newVariant;
			map<TuningParameter*,int> newMap;			

			// Since we know we have two TP, can replace by vector iterator
			for(std::size_t i=0; i<tuningPointVec.size(); i++ ) {
				newMap[tuningPointVec[i]] = tuningParameter[i];
			}
			
			newVariant.setValue(newMap);

			string uniqueConfig = toString(newVariant, tuningPointVec);
			
			if(populElem.count(uniqueConfig) == 1) { //Already in population
				continue;
			} 
			
			populElem.insert(uniqueConfig);
			population.push_back(newVariant);
			membertoCreate--;
		}			
	}
	
	// Generate new population according to GDE3 population generation, even works for first random iteration
	// TODO: current problem is Variant can store only integer values, can be problematic afterwards
	int popSize = population.size();	
	int idx = 0;
	while(idx < popSize)
	{
		// For every member of population
		Variant newChildVariant;
		map<TuningParameter*,int> newChildMap;
		
		// Choose parent
		map<TuningParameter*,int> parentMap = population[idx].getValue();
		
		// Choose 3 random members from population
		int random[3] = {-1,-1,-1};
		int i=0;
		while( i < 3) {
			int newEntry = rand() % population.size();
			if(newEntry != random[((i+1)%3)] && newEntry != random[((i-1)%3)]) {
				random[i++] = newEntry;
			}
		}
		
//		set<int> randMem;
//		while(randMem.size()!= 3) {
//			int newEntry = rand() % population.size();
//			if( randMem.count(newEntry) != 1 && newEntry != idx) {
//				randMem.insert(newEntry);
//			}
//		}		
//		set<int>::iterator iter = randMem.begin();		
		// Get the maps for all these randomly chosen members
//		map<TuningPoint*,int> rOneMap = population[*iter++].getValue();
//		map<TuningPoint*,int> rTwoMap = population[*iter++].getValue();
//		map<TuningPoint*,int> rThreeMap = population[*iter].getValue();

		map<TuningParameter*,int> rOneMap = population[random[0]].getValue();
		map<TuningParameter*,int> rTwoMap = population[random[1]].getValue();
		map<TuningParameter*,int> rThreeMap = population[random[2]].getValue();

		size_t randIndex = floor(rand()%tuningPointVec.size()) + 1;
		
		// iterate over all TP of variant
		for(std::size_t j=0; j<tuningPointVec.size(); j++) {
			if( rand() < CR || j == randIndex ) {
				newChildMap[tuningPointVec[j]] = rThreeMap[tuningPointVec[j]] + F*(rOneMap[tuningPointVec[j]] - rTwoMap[tuningPointVec[j]]);
			}
			else {
				newChildMap[tuningPointVec[j]] = parentMap[tuningPointVec[j]];
			}				
		}
		
		// Add random configuration to the pool
		newChildVariant.setValue(newChildMap);
		
		// Need to make sure there are no repetitions in the population
		string uniqueConfig = toString(newChildVariant,tuningPointVec);
		if(populElem.count(uniqueConfig) == 1) { // config already present, redo
//			cout << "Found... COntinue Idx:" << idx << endl;
//			cout << "------------------------ After idx --" << endl;
			continue;
		}
		idx++;		
		populElem.insert(uniqueConfig);
		population.push_back(newChildVariant);

		logString.append("(" + toString(population[idx-1],tuningPointVec) + "),");
		logString.append("(" + toString(newChildVariant,tuningPointVec) + "),");		
		
		// Add reference in parent child map for evaluate function
		parentChildMap[idx-1] = population.size()-1;		
	}
	logString.append("}");
}
 

/*
 * Evaluate if the population from last 3 iterations are same OR
 * maximum no of iterations reached
 */
bool Gde3Algorithm::isSearchFinished() const {
	return searchFinishedVar;
}


int Gde3Algorithm::getOptimum() {
	//printing optimal value here
	stringstream ssObjVal;
	ssObjVal << optimalObjVal;
	logString.append( "\nOptimal value found in iteration: " + ssObjVal.str() + "\n");
	
	return 0;
}

void Gde3Algorithm::setSearchFinished( bool searchFinished ) {
	this->searchFinishedVar = searchFinished;
}

bool Gde3Algorithm::checkFeasible(Variant & varEval) {
	
	bool vectorFeasible = true;
	vector<TuningParameter*>::iterator iter;
	map<TuningParameter*,int> vectormap = varEval.getValue();
	
	for(iter = tuningPointVec.begin();iter != tuningPointVec.end(); ++iter) {		
//		const TuningPointRange & tuningPointRange = (*iter)->getTpRange();		
		if(	vectormap[*iter] < (*iter)->getRangeFrom()/*tuningPointRange.getMin()*/ ||
			vectormap[*iter] > (*iter)->getRangeTo()/*tuningPointRange.getMax()*/ ) {
			vectorFeasible = false;
			break;
		}
	}	
	return vectorFeasible;
}

int Gde3Algorithm::compareVariants(Variant & parent, Variant & child,map< string,vector<double> > & evalVec) {
	int result;
	
	vector<double> parentMap = evalVec[toString(parent,tuningPointVec)];
	vector<double> childMap = evalVec[toString(child,tuningPointVec)];
	
	if(parentMap.size() != childMap.size()) {
		cout << "Fatal Erorr !" << endl;
		return -100;
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
	for(int i=0; i<parentMap.size(); i++) {
		if(parentMap[i] > childMap[i]) { // Minimization problem
			parentDom = false;
			break;
		}
	}
	
	if(parentDom) {
		return -1;
	}
	
	// compare for child dominance
	bool childDom = true;
	for(int i=0; i<parentMap.size(); i++) {
		if(parentMap[i] < childMap[i]) { // Minimization problem
			childDom = false;
			break;
		}
	}
	
	if(childDom) {
		return 1;
	}
	
	return 0; // non dominance of both	
}

void Gde3Algorithm::checkSearchFinished( map< string,vector<double> > & evalVec ) {
		 	
	// evaluate all configurations in population	
	for(std::size_t i=0; i<population.size(); i++)
	{	
		if(parentChildMap.count(i) > 0 ) { // parent
			int parent = i;
			int child = parentChildMap[i];
			
			// send for evaluation
			int resultComp = compareVariants(population[parent], population[child], evalVec);
			
			if( resultComp == 0) { // both dominant
				tobeDropped.insert(child);
			} else if(resultComp < 0) { //Parent Dominant
				tobeDropped.insert(child);
			} else if( resultComp > 0) { // Child Dominant
				tobeDropped.insert(parent);
			}				
		}
	}
	
	cleanupPopulation(evalVec);
	
	// if the population has more elements than population size, clear depending on Crowding distance
	if(population.size() > populationSize) {
		bool resizingDone = false;
		for(std::size_t i=0; i < population.size(); i++) {
			for(std::size_t j=i+1; j < population.size(); j++) {
				
				// send for evaluation
				int resultComp = compareVariants(population[i], population[j],evalVec);
				
				if( resultComp == 0) { // both dominant
					// do nothing, cannot lose dominant vectors
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
							// Logic for crowding distance
		}
		
		// clear elements from population vector
		cleanupPopulation(evalVec);
	}
	
	// Refill the unique set again, so that there are no repetition of configurations	
	for( vector<Variant>::iterator iter = population.begin(); iter != population.end(); ++iter) {		
		map<TuningParameter*,int> vectorMap = (*iter).getValue();
		string uniqueElem = toString(*iter,tuningPointVec);
		populElem.insert(uniqueElem);
	}
	
	// Priting evaluation of this generation, later this code can be removed
	logString.append("Objective values after generation:\n {" );
	for(vector<Variant>::iterator iter= population.begin();iter!= population.end(); ++iter) {
		
//		vector<double> objVal = evalObjFunction(*iter, algorithm);
		vector<double> objVal = evalVec[toString(*iter,tuningPointVec)];
		
		logString.append("(");
		for(size_t i=0; i<objVal.size(); i++) {
			stringstream ss1;
			ss1 << objVal[i];
			logString.append( ss1.str() + ",");
		}
		logString.append(")");
	}
	
	logString.append("}\n");

	
	if(recentPopulation.empty()) { // first time
		vector<Variant>::iterator iter;
		for(iter=population.begin(); iter != population.end(); ++iter) {
			recentPopulation.push_back(*iter);
		}			
	}
	
	bool generationEqual = true;
	
	// compare two population vectors
	if(population.size() == recentPopulation.size()) {
		for(std::size_t i=0; i< recentPopulation.size(); i++) {
			if( toString(population[i],tuningPointVec) != toString(recentPopulation[i],tuningPointVec) ){
				generationEqual = false;
				break;
			}
		}
		
		if(generationEqual) {
			samePopulationVector++;
		}
	}
	
	// replace replacePopulation by newest one
	recentPopulation.clear();
	vector<Variant>::iterator iter;
	for(iter=population.begin(); iter != population.end(); ++iter) {
		recentPopulation.push_back(*iter);
	}					
	if(samePopulationVector == 3) {
		setSearchFinished(true);		
	}	
}

void Gde3Algorithm::cleanupPopulation(map< string,vector<double> > & evalVec) {
	
	// End of iteration, cleanup mess
	vector<Variant> popuNextGen;
	for(std::size_t i=0; i< population.size(); i++) {		
		if(tobeDropped.count(i) != 1 ) { //not found
			popuNextGen.push_back(population[i]);
		} else {
			// found in tobeDropped, remove from evalVec
			evalVec.erase(toString(population[i],tuningPointVec));
		}
	}
	
	population.clear();
	population = popuNextGen;
	parentChildMap.clear();
	tobeDropped.clear();
	populElem.clear();
}

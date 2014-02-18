#include "Gde3Algorithm.h"

Gde3Algorithm::Gde3Algorithm(){

	// Minimization problem
	optimalObjVal = 100000.0;
	
	// Set initial population size
	populationSize = 10;	
	samePopulationVector = 0;
	
	CR = 0.5;
	F = 0.5;
}

Gde3Algorithm::~Gde3Algorithm() {
	// destroy all dynamically allocated memory
	delete searchSpace;
	
	vector<TuningParameter*>::iterator iter;
}

/***************************************************************************
 * Adds search space to local pointer to search space
 **************************************************************************/
void Gde3Algorithm::addSearchSpace( SearchSpace * space ) {
	this->searchSpace = space;
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


/***************************************************************************
 * For first time, creates random population
 * Creates new configurations
 **************************************************************************/
void Gde3Algorithm::createScenarios() {
	
	logString.append("\n\nGeneration Population: \n { ");
	
	// initialize random seed
	srand(time(NULL));
	int membertoCreate = populationSize;
	
	vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters(); 
			
	if(population.empty()) { // first time in create Scenarios		
		while(membertoCreate) {
			
			Variant newVariant;
			map<TuningParameter*,int> newMap;
			
			for(std::size_t i=0; i<tuningParVec.size(); i++ ) {
				int value = rand()%populationSize + tuningParVec[i]->getRangeFrom();
				
				// TODO: The value must round off to nearest tuningParVec[i]->getRangeStep() 
				// if all values are going to be integers, then no need !
				
				newMap[tuningParVec[i]] = value;
			}
			
			newVariant.setValue(newMap);

			string uniqueConfig = toString(newVariant, tuningParVec);
			
			if(populElem.count(uniqueConfig) == 1) { //Already in population
				continue;
			} 
			
			populElem.insert(uniqueConfig);
			population.push_back(newVariant); // TODO: replace by adding to scenario pool
			membertoCreate--;
		}			
	}
	
	// Generate new population according to GDE3 population generation, even works for first random iteration
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
			if(newEntry != random[((i+1)%3)] && newEntry != random[((i-1)%3)]) { // ensuring no repeat
				random[i++] = newEntry;
			}
		}
		
		map<TuningParameter*,int> rOneMap = population[random[0]].getValue();
		map<TuningParameter*,int> rTwoMap = population[random[1]].getValue();
		map<TuningParameter*,int> rThreeMap = population[random[2]].getValue();

		size_t randIndex = floor(rand()%tuningParVec.size()) + 1;
		
		// iterate over all TP of variant
		for(std::size_t j=0; j<tuningParVec.size(); j++) {
			if( rand() < CR || j == randIndex ) {
				newChildMap[tuningParVec[j]] = rThreeMap[tuningParVec[j]] + F*(rOneMap[tuningParVec[j]] - rTwoMap[tuningParVec[j]]);
			}
			else {
				newChildMap[tuningParVec[j]] = parentMap[tuningParVec[j]];
			}				
		}
		
		newChildVariant.setValue(newChildMap);
		
		// Need to make sure there are no repetitions in the population
		string uniqueConfig = toString(newChildVariant,tuningParVec);
		if(populElem.count(uniqueConfig) == 1) { // config already present, redo
			continue;
		}
		
		idx++;		
		populElem.insert(uniqueConfig);
		population.push_back(newChildVariant);// TODO:replace by adding to scenario pool

		logString.append("(" + toString(population[idx-1],tuningParVec) + "),");
		logString.append("(" + toString(newChildVariant,tuningParVec) + "),");		
		
		// Add reference in parent child map for evaluate function
		parentChildMap[idx-1] = population.size()-1;		
	}
	logString.append("}");
}

int Gde3Algorithm::getOptimum() {
	//printing optimal value here
	stringstream ssObjVal;
	ssObjVal << optimalObjVal;
	logString.append( "\nOptimal Value found in iteration: " + ssObjVal.str() + "\n");
	logString.append( "Optimal Variant found in iteration: (" + this->optimalVariant + ")\n");
	
	return 0;
}

bool Gde3Algorithm::checkFeasible(Variant & varEval) {
	
	bool vectorFeasible = true;
	
	vector<TuningParameter*>::iterator iter;
	map<TuningParameter*,int> vectormap = varEval.getValue();
	vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
	
	for(iter = tuningParVec.begin();iter != tuningParVec.end(); ++iter) { // may replace iter here by index of tuningParVec		
		if(	vectormap[*iter] < (*iter)->getRangeFrom() ||
			vectormap[*iter] > (*iter)->getRangeTo() ) {
			vectorFeasible = false;
			break;
		}
	}	
	return vectorFeasible;
}

int Gde3Algorithm::compareVariants(Variant & parent, Variant & child, map< string,vector<double> > & evalVec) {

	vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
	vector<double> parentMap = evalVec[toString(parent,tuningParVec)];
	vector<double> childMap = evalVec[toString(child,tuningParVec)];
	
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

bool Gde3Algorithm::searchFinished( map< string,vector<double> > & evalVec ) {
	
	bool searchFinish = false;
	vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
	
	// evaluate all configurations in population	
	for(std::size_t i=0; i<population.size(); i++)
	{	
		if(parentChildMap.count(i) > 0 ) { // parent
			int parent = i;
			int child = parentChildMap[i];
			
			// send for evaluation
			int resultComp = compareVariants(population[parent], population[child], evalVec);
			
			if( resultComp == 0) { // both dominant
				tobeDropped.insert(child); // TODO: Remove after implemeting crowding distance logic, for multiple objective function
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
		cleanupPopulation(evalVec);
	}
	
	// Refill the unique set again, so that there are no repetition of configurations	
	for( vector<Variant>::iterator iter = population.begin(); iter != population.end(); ++iter) {		
		map<TuningParameter*,int> vectorMap = (*iter).getValue();
		string uniqueElem = toString(*iter,tuningParVec);
		populElem.insert(uniqueElem);		
	}
	
	// Print Population after cleaning
	logString.append("Cleaned Population: \n { ");
	for(vector<Variant>::iterator iter= population.begin();iter!= population.end(); ++iter) {
		logString.append("(");
		logString.append(toString(*iter, tuningParVec));
		logString.append(") , ");
	}
	logString.append("}\n");
	
	// Priting evaluation of this generation, later this code can be removed
	logString.append("Objective values after generation:\n {" );
	for(vector<Variant>::iterator iter= population.begin();iter!= population.end(); ++iter) {
		
		vector<double> objVal = evalVec[toString(*iter,tuningParVec)];
		
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
			if( toString(population[i],tuningParVec) != toString(recentPopulation[i],tuningParVec) ){
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
		searchFinish = true;
	}	
	return searchFinish;
}

void Gde3Algorithm::cleanupPopulation(map< string,vector<double> > & evalVec) {
	
	// End of iteration, cleanup mess
	vector<Variant> popuNextGen;
	vector<TuningParameter*> tuningParVec = this->searchSpace->getVariantSpace()->getTuningParameters();
	
	for(std::size_t i=0; i< population.size(); i++) {		
		if(tobeDropped.count(i) != 1 ) { //not found
			popuNextGen.push_back(population[i]);
		} else {
			// found in tobeDropped, remove from evalVec
			evalVec.erase(toString(population[i],tuningParVec));
		}
	}
	
	population.clear();
	population = popuNextGen;
	parentChildMap.clear();
	tobeDropped.clear();
	populElem.clear();
}

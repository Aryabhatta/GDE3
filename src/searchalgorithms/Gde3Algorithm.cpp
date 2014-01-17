#include "Gde3Algorithm.h"

Gde3Algorithm::Gde3Algorithm()
{
	// Minimization problem
	optimalObjVal = 100000.0;
	
	// Set initial population size
	populationSize = 10;	
	searchFinished = false;
	
	CR = 0.5;
	F = 0.5;
}

Gde3Algorithm::~Gde3Algorithm()
{
	// destroy all dynamically allocated memory
	delete variantSpace;
	vector<TuningPoint*>::iterator iter;
	for( iter = tuningPointVec.begin(); iter != tuningPointVec.end(); ++iter) {
		delete *iter;
	}
}

TuningPoint * Gde3Algorithm::createTuningPoint(long id, string tuningActionName, tunableType parameterType, int minRange,
			  					int maxRange) {
	// Adding first parameter
	TuningPoint * tuningPointNew = new TuningPoint();
	tuningPointNew->setId(id);
	tuningPointNew->setTuningActionName(tuningActionName);
	tuningPointNew->setParameterType(parameterType);
	
	TuningPointRange tuningPointNewRange(minRange,maxRange); 
	tuningPointNew->setTpRange(tuningPointNewRange);
	
	// TODO: Currently, not specified anything as restriction, to change
	Restriction restriction;
	tuningPointNew->setRestriction(restriction);
	
	if(true) {
	cout << "Tuning plugin create with id: " << id << " Name: " << tuningActionName << " Parameter type: "; 
	cout << parameterType << " TuningPointRange: " << minRange << " to " << maxRange << endl;
	}
	
	return tuningPointNew;
}

/***************************************************************************
 * Adds search space to local pointer to search space
 **************************************************************************/
void Gde3Algorithm::addSearchSpace( VariantSpace * space ) {
	variantSpace = space;
}

string toString( Variant & variant, vector<TuningPoint*> & TuningPointVec ) {
	map<TuningPoint*, int > vectorMap = variant.getValue();	
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
			map<TuningPoint*,int> newMap;			

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
		map<TuningPoint*,int> newChildMap;
		
		// Choose parent
		map<TuningPoint*,int> parentMap = population[idx].getValue();
		
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

		map<TuningPoint*,int> rOneMap = population[random[0]].getValue();
		map<TuningPoint*,int> rTwoMap = population[random[1]].getValue();
		map<TuningPoint*,int> rThreeMap = population[random[2]].getValue();

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
	return searchFinished;
}


int Gde3Algorithm::getOptimum() {
	//printing optimal value here
	stringstream ssObjVal;
	ssObjVal << optimalObjVal;
	logString.append( "\nOptimal value found in iteration: " + ssObjVal.str() + "\n");
	
	return 0;
}

void Gde3Algorithm::setSearchFinished( bool searchFinished ) {
	this->searchFinished = searchFinished;
}
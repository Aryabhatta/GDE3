#include "GdeAlgorithm.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
using namespace std;

#define PrintLevel 1

GdeAlgorithm::GdeAlgorithm()
{
	// Minimization problem
	optimalObjVal = 100000.0;
	
	// Set initial population size
	populationSize = 10;	
	searchFinished = false;
	
	CR = 0.5;
	F = 0.5;
}

GdeAlgorithm::~GdeAlgorithm()
{
	// destroy all dynamically allocated memory
	delete variantSpace;
	delete tuningPointVec[0];
	delete tuningPointVec[1];
}

TuningPoint * GdeAlgorithm::createTuningPoint(long id, string tuningActionName, tunableType parameterType, int minRange,
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
	
	if(PrintLevel) {
	cout << "Tuning plugin create with id: " << id << " Name: " << tuningActionName << " Parameter type: "; 
	cout << parameterType << " TuningPointRange: " << minRange << " to " << maxRange << endl;
	}
	
	return tuningPointNew;
}

/***************************************************************************
 * Creates TuningPoints  
 **************************************************************************/
VariantSpace * GdeAlgorithm::initialize() {
	
	VariantSpace * varSpace = new VariantSpace();
	
	// Adding two parameters to the map
	map<TuningPoint*, TuningPointConstraint> space;
	
	TuningPoint * tuningPointOne = createTuningPoint(100, "X value", CODE_REGION, -5, 5);
	
	// Can inherit Constraint from TuningPointConstraint
	TuningPointConstraint tuningPointOneConstraint;
	tuningPointOneConstraint.setLeftBoundary(-5);
	tuningPointOneConstraint.setRightBoundary(5);
	
	space[tuningPointOne] = tuningPointOneConstraint;
	tuningPointVec.push_back(tuningPointOne);	
	
	// Adding second parameter
	TuningPoint * tuningPointTwo = createTuningPoint(200, "Y value", CODE_REGION, -5, 5);
	
	// Can inherit Constraint from TuningPointConstraint
	TuningPointConstraint tuningPointTwoConstraint;
	tuningPointTwoConstraint.setLeftBoundary(-5);
	tuningPointTwoConstraint.setRightBoundary(5);
	
	space[tuningPointTwo] = tuningPointTwoConstraint;
	tuningPointVec.push_back(tuningPointTwo);
	
	varSpace->setSpace(space);	

	return varSpace;
}

/***************************************************************************
 * Adds search space to local pointer to search space
 **************************************************************************/
void GdeAlgorithm::addSearchSpace( VariantSpace * space ) {
	variantSpace = space;
}

template <class T> 
string toString( T one, T two )
{
	string newString;
	
	stringstream s1, s2;
	s1 << one; s2 << two;
	newString = s1.str() + "," + s2.str();
	return newString;
}

/***************************************************************************
 * For first time, creates random population
 * Creates new configurations
 **************************************************************************/
void GdeAlgorithm::createScenarios() {
	
	string logString = "\n\nGeneration Population: \n { ";
	
	// initialize random seed
	srand(time(NULL));
	int membertoCreate = populationSize;
			
	if(population.empty()) { // first time in create Scenarios		
		while(membertoCreate) {
			
			vector<int> tuningParameter;
			
			for(int i=0; i<tuningPointVec.size(); i++ ) {
				int value = rand()%populationSize + (-5);
				tuningParameter.push_back(value);
			}
			
			string uniqueConfig = toString(tuningParameter[0],tuningParameter[1]);
			
			if(populElem.count(uniqueConfig) != 1) { //not there in population
				populElem.insert(uniqueConfig);
			} else {
				continue;
			}
		
			Variant newVariant;
			map<TuningPoint*,int> newMap;			

			// Since we know we have two TP, can replace by vector iterator
			for(int i=0; i<tuningPointVec.size(); i++ ) {
				newMap[tuningPointVec[i]] = tuningParameter[i];
			}
			
			newVariant.setValue(newMap);
			population.push_back(newVariant);
			membertoCreate--;
		}			
	}
	
	// Generate new population according to GDE3 population generation, even works for first random iteration
	// TODO: current problem is Variant can store only integer values, can be problematic afterwards
	int popSize = population.size();
	for(int idx=0; idx < popSize;idx++)
	{
		// For every member of population
		Variant newChildVariant;
		map<TuningPoint*,int> newChildMap;
		
		// Choose parent
		map<TuningPoint*,int> parentMap = population[idx].getValue();
		
		// Choose 3 random members from population
		set<int> randMem;
		while(randMem.size()!= 3) {
			int newEntry = rand() % population.size();
			if( randMem.count(newEntry) != 1 && newEntry != idx) {
				randMem.insert(newEntry);
			}
		}
		
		set<int>::iterator iter = randMem.begin();
		
		// Get the maps for all these randomly chosen members
		map<TuningPoint*,int> rOneMap = population[*iter++].getValue();
		map<TuningPoint*,int> rTwoMap = population[*iter++].getValue();
		map<TuningPoint*,int> rThreeMap = population[*iter].getValue();
		
		int randIndex = floor(rand()%tuningPointVec.size()) + 1;
		
		// iterate over all TP of variant
		for(int j=0; j<tuningPointVec.size(); j++) {
			if( rand() < CR || j==randIndex ) {
				newChildMap[tuningPointVec[j]] = rThreeMap[tuningPointVec[j]] + F*(rOneMap[tuningPointVec[j]] - rTwoMap[tuningPointVec[j]]);
			}
			else {
				newChildMap[tuningPointVec[j]] = parentMap[tuningPointVec[j]];
			}				
		}
		
		// Add random configuration to the pool
		newChildVariant.setValue(newChildMap);
		
		// Need to make sure there are no repetitions in the population
		string uniqueConfig = toString(newChildMap[tuningPointVec[0]],newChildMap[tuningPointVec[1]]);
		if(populElem.count(uniqueConfig) == 1) { // config already present, redo
			idx--; // adjusting counter for next loop
			continue;
		}
		
		
		logString += "(" + toString(parentMap[tuningPointVec[0]],parentMap[tuningPointVec[1]]) + "),";
		logString += "(" + toString(newChildMap[tuningPointVec[0]],newChildMap[tuningPointVec[1]]) + "),";
		
		population.push_back(newChildVariant);
		
		// Add reference in parent child map for evaluate function
		parentChildMap[idx] = population.size()-1;
	}
	
	logString += "}";
	
	if(PrintLevel) {
	cout << logString << endl;
	}
}
 
void GdeAlgorithm::evaluate(int parentIdx, int childIdx) {	// This contains the actual evaluation of our population
	
	Variant & parent = population[parentIdx];
	Variant & child = population[childIdx];	
	
	// check for feasibility first, make sure it obeys all constraints
	if(!checkFeasible(parent)) {		
		// if not feasible, drop it from population
		tobeDropped.push_back(parentIdx);
		return;
	}
	
	// check for feasibility first, make sure it obeys all constraints
	if(!checkFeasible(child)) {		
		// if not feasible, drop it from population
		tobeDropped.push_back(childIdx);
		return;
	}
	
	// both are feasible, check dominant
	map<TuningPoint*, int> parentMap = parent.getValue();
	map<TuningPoint*, int> childMap = child.getValue();
	
	int xParent = parentMap[tuningPointVec[0]];
	int yParent = parentMap[tuningPointVec[1]];
	int xChild = childMap[tuningPointVec[0]];
	int yChild = childMap[tuningPointVec[1]];
	
	/* Computing Ackley's function (Objective function)
	 * f(x,y) = -20 exp(-0.2 * Sqrt(0.5*(x^2+y^2))) - exp(0.5 * (cos(2*pi*x) + cos(2*pi*y))) + 20 + e 
	 * 
	 */
	double objValuePar = -20 * exp(-0.2 * (sqrt(0.5 * (xParent*xParent+yParent*yParent)))) - exp(0.5 * (cos(2*M_PI*xParent) + cos(2*M_PI*yParent))) + 20 + M_E;
	double objValueChd = -20 * exp(-0.2 * (sqrt(0.5 * (xChild*xChild+yChild*yChild)))) - exp(0.5 * (cos(2*M_PI*xChild) + cos(2*M_PI*yChild))) + 20 + M_E;
	
	// TODO: specific to problem, this is a minimization problem
	if( objValuePar == objValueChd ) { // both are non-dominant, decide what to do
		tobeDropped.push_back(childIdx); // TODO: For time being, change upon Crowding Distance 
	} else if( objValuePar < objValueChd ) {
		tobeDropped.push_back(childIdx);		
	} else {
		tobeDropped.push_back(parentIdx);
	}

	cout << objValuePar << "," << objValueChd << ",";
	
	// Minimisation problem: get the value & compare it with population optimal
	if(std::min(objValuePar,objValueChd) < optimalObjVal) {
		optimalObjVal = std::min(objValuePar,objValueChd);		
	}
	
	return;
}

double GdeAlgorithm::evaluate(Variant & member) {
	
	map<TuningPoint*, int> vectorMap = member.getValue();
	
	
	int x = vectorMap[tuningPointVec[0]];
	int y = vectorMap[tuningPointVec[1]];
	
	double objValue = -20 * exp(-0.2 * (sqrt(0.5 * (x*x+y*y)))) - exp(0.5 * (cos(2*M_PI*x) + cos(2*M_PI*y))) + 20 + M_E;
	return objValue;
	
}

/*
 * Evaluate if the population from last 3 iterations are same OR
 * maximum no of iterations reached
 */
bool GdeAlgorithm::isSearchFinished() const {	
	return searchFinished;
}

bool GdeAlgorithm::checkFeasible(Variant & varEval ) {
	
	bool vectorFeasible = true;
	vector<TuningPoint*>::iterator iter;
	map<TuningPoint*, TuningPointConstraint> space = variantSpace->getSpace();
	map<TuningPoint*,int> vectormap = varEval.getValue();
	for(iter = tuningPointVec.begin();iter!=tuningPointVec.end(); ++iter) {
		TuningPointConstraint & constraint = space[*iter];
		if(	vectormap[*iter] < constraint.getLeftBoundary() ||
			vectormap[*iter] > constraint.getRightBoundary() ) {
			vectorFeasible = false;
			break;
		}
	}	
	return vectorFeasible;
}
/*
 * Evaluate whole population and discard weaker
 * between child and parent
 */
void GdeAlgorithm::doSearch() {
	
	cout << "Evaluation:" << endl << "{";
	
	// evaluate all configurations in population	
	for( int i=0; i<population.size(); i++)
	{
		Variant & varEval = population[i];
		
		if(parentChildMap.count(i) > 0 ) { // parent
			int child = parentChildMap[i];
			
			// send for evaluation, TODO: what in case of multiple objective values ?
			evaluate(i, child);
		}
	}
	
	cout << " }" << endl << endl;
	
	// End of iteration, cleanup mess
	vector<Variant> popuNextGen;
	for( int i=0; i< population.size(); i++) {
		
		if(std::find(tobeDropped.begin(), tobeDropped.end(),i) == tobeDropped.end()) {
			popuNextGen.push_back(population[i]);
		}		
	}
	population.clear();
	population = popuNextGen;
	parentChildMap.clear();
	tobeDropped.clear();
	
	// Refill the unique set again, so that there are no repetition of configurations
	populElem.clear();
	for( vector<Variant>::iterator iter = population.begin(); iter != population.end(); ++iter) {		
		map<TuningPoint*,int> vectorMap = (*iter).getValue();
		string uniqueElem = toString(vectorMap[tuningPointVec[0]],vectorMap[tuningPointVec[1]]);
		populElem.insert(uniqueElem);
	}
	
	// Priting evaluation of this generation, later this code can be removed
	cout << "Objective values after generation:" << endl << "{";
	for(vector<Variant>::iterator iter= population.begin();iter!= population.end(); ++iter) {
		Variant & member = *iter;
		cout << evaluate(*iter) << "," ;
		
	}
	cout << "}" << endl;
	
	// In case of Ackley's function
	if(optimalObjVal < 0 ) {
		this->searchFinished = true;
	}
}

int GdeAlgorithm::getOptimum() {
	//printing optimal value here
	cout << endl << "Optimal value found in iteration: " << optimalObjVal << endl;
	
	return 0;
}

void GdeAlgorithm::setSearchFinished( bool searchFinished ) {
	this->searchFinished = searchFinished;
}
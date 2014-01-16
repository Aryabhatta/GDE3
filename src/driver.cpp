//============================================================================
// Name        : GDE3.cpp
// Author      : ShrikantV
// Version     :
// Copyright   : Your copyright notice
// Description : GDE3 Implementation
//============================================================================

#include <iostream>
#include <sstream>
#include "searchalgorithms/Gde3Algorithm.h"
using namespace std;

// Map for reducing the no of evaluations
std::map<string,double> evalVec;
vector<Variant> recentPopulation;
int samePopulationVector = 0;
string toString( Variant & variant, vector<TuningPoint*> & TuningPointVec );

bool checkFeasible(Variant & varEval, Gde3Algorithm & algorithm ) {
	
	bool vectorFeasible = true;
	vector<TuningPoint*>::iterator iter;
	map<TuningPoint*,int> vectormap = varEval.getValue();
	
	for(iter = algorithm.tuningPointVec.begin();iter != algorithm.tuningPointVec.end(); ++iter) {		
		
		const TuningPointRange & tuningPointRange = (*iter)->getTpRange();		
		if(	vectormap[*iter] < tuningPointRange.getMin() ||
			vectormap[*iter] > tuningPointRange.getMax() ) {
			vectorFeasible = false;
			break;
		}
	}	
	return vectorFeasible;
}

std::vector<double> evalObjFunction(Variant & variant, Gde3Algorithm & algorithm) {
	
	vector<double> result;
	
	// List all Tuning Parameters
	map<TuningPoint*,int> vectormap = variant.getValue();
	vector<int> tuningParam;
	
	for(std::size_t i=0; i < algorithm.tuningPointVec.size(); i++) {
		int value = vectormap[algorithm.tuningPointVec[i]];
		tuningParam.push_back(value);
	}	
		
	double objFunc1 = -20 * exp(-0.2 * (sqrt(0.5 * (tuningParam[0]*tuningParam[0]+tuningParam[1]*tuningParam[1])))) 
	                  - exp(0.5 * (cos(2*M_PI*tuningParam[0]) + cos(2*M_PI*tuningParam[1]))) + 20 + M_E;
	
	result.push_back(objFunc1);	
	return result;
}

void evaluate(int parentIdx, int childIdx, Gde3Algorithm & algorithm) {	// This contains the actual evaluation of our population
	
	Variant & parent = algorithm.population[parentIdx];
	Variant & child = algorithm.population[childIdx];	
	
	// check for feasibility first, make sure it obeys all constraints
	if(!checkFeasible(parent,algorithm)) {		
		// if not feasible, drop it from population
		algorithm.tobeDropped.push_back(parentIdx);
		return;
	}
	
	// check for feasibility first, make sure it obeys all constraints
	if(!checkFeasible(child,algorithm)) {		
		// if not feasible, drop it from population
		algorithm.tobeDropped.push_back(childIdx);
		return;
	}
	
	// both are feasible, check dominant
	map<TuningPoint*, int> parentMap = parent.getValue();
	map<TuningPoint*, int> childMap = child.getValue();
	
	int xParent = parentMap[algorithm.tuningPointVec[0]];
	int yParent = parentMap[algorithm.tuningPointVec[1]];
	int xChild = childMap[algorithm.tuningPointVec[0]];
	int yChild = childMap[algorithm.tuningPointVec[1]];
	
	/* Computing Ackley's function (Objective function)
	 * f(x,y) = -20 exp(-0.2 * Sqrt(0.5*(x^2+y^2))) - exp(0.5 * (cos(2*pi*x) + cos(2*pi*y))) + 20 + e 
	 * 
	 */
	
	double objValuePar = 0.0;
	double objValueChd = 0.0;
	
	string parentStr = toString(parent,algorithm.tuningPointVec);
	string childStr = toString(child,algorithm.tuningPointVec);
	
	//evaluating only if necessary
	if(evalVec.count(parentStr) != 1 ) {
		objValuePar = -20 * exp(-0.2 * (sqrt(0.5 * (xParent*xParent+yParent*yParent)))) - exp(0.5 * (cos(2*M_PI*xParent) + cos(2*M_PI*yParent))) + 20 + M_E;
		evalVec[parentStr] = objValuePar;
	} else {
		objValuePar = evalVec[parentStr];
	}

	// evaluations only when necessary
	if(evalVec.count(childStr) != 1) {
		objValueChd = -20 * exp(-0.2 * (sqrt(0.5 * (xChild*xChild+yChild*yChild)))) - exp(0.5 * (cos(2*M_PI*xChild) + cos(2*M_PI*yChild))) + 20 + M_E;
		evalVec[childStr] = objValueChd;
	} else {
		objValueChd = evalVec[childStr];
	}

	
	// TODO: specific to problem, this is a minimization problem
	if( objValuePar == objValueChd ) { // both are non-dominant, decide what to do 
		algorithm.tobeDropped.push_back(childIdx); // TODO: For time being, change upon Crowding Distance
										 			// Not happening for Ackley's function
	} else if( objValuePar < objValueChd ) {
		algorithm.tobeDropped.push_back(childIdx);
	} else {
		algorithm.tobeDropped.push_back(parentIdx);
	}

	stringstream ss1, ss2;
	ss1 << objValuePar; ss2 << objValueChd;
	algorithm.logString.append( ss1.str() + "," + ss2.str() + ",");
	
	// Minimisation problem: get the value & compare it with population optimal
	if(std::min(objValuePar,objValueChd) < algorithm.optimalObjVal) {
		algorithm.optimalObjVal = std::min(objValuePar,objValueChd);		
	}	
	return;
}

double evaluate(Variant & member, Gde3Algorithm & algorithm) {
	
	map<TuningPoint*, int> vectorMap = member.getValue();
	
	
	int x = vectorMap[algorithm.tuningPointVec[0]];
	int y = vectorMap[algorithm.tuningPointVec[1]];
	
	double objValue = -20 * exp(-0.2 * (sqrt(0.5 * (x*x+y*y)))) - exp(0.5 * (cos(2*M_PI*x) + cos(2*M_PI*y))) + 20 + M_E;
	return objValue;
	
}
void cleanupPopulation( Gde3Algorithm & algorithm) {
	
	// End of iteration, cleanup mess
	vector<Variant> popuNextGen;
	for(std::size_t i=0; i< algorithm.population.size(); i++) {		
		if(std::find(algorithm.tobeDropped.begin(), algorithm.tobeDropped.end(),i) == algorithm.tobeDropped.end()) { //not found in tobeDropped
			popuNextGen.push_back(algorithm.population[i]);
		} else {
			// found in tobeDropped, remove from evalVec
			evalVec.erase(toString(algorithm.population[i],algorithm.tuningPointVec));
		}
	}
	
	algorithm.population.clear();
	algorithm.population = popuNextGen;
	algorithm.parentChildMap.clear();
	algorithm.tobeDropped.clear();
	algorithm.populElem.clear();
}


void evaluatePopulation(Gde3Algorithm & algorithm) {
	algorithm.logString.append("\n Evaluation: \n {" );
	
	// evaluate all configurations in population	
	for(std::size_t i=0; i<algorithm.population.size(); i++)
	{
		if(algorithm.parentChildMap.count(i) > 0 ) { // parent
			int child = algorithm.parentChildMap[i];
			
			// send for evaluation, TODO: what in case of multiple objective values ?
			evaluate(i, child, algorithm);
		}
	}
	
	algorithm.logString.append(" }\n\n");
	
	cleanupPopulation(algorithm);
	
	// if the population has more elements than population size, clear depending on Crowding distance
	if(algorithm.population.size() > algorithm.populationSize) {
		bool resizingDone = false;
		for(std::size_t i=0; i < algorithm.population.size(); i++) {
			for(std::size_t j=i+1; j < algorithm.population.size(); j++) {
				if(std::find(algorithm.tobeDropped.begin(), algorithm.tobeDropped.end(),i) != algorithm.tobeDropped.end() &&
				   std::find(algorithm.tobeDropped.begin(), algorithm.tobeDropped.end(),j) != algorithm.tobeDropped.end()	) {
					// if both i,j are not present in tobeDroppedlist, then only evaluate
					evaluate(i,j, algorithm);				
					
					if(algorithm.population.size()-algorithm.tobeDropped.size()==algorithm.populationSize) {
						resizingDone = true;
						break;
					}						
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
		cleanupPopulation(algorithm);
	}
	
	// Refill the unique set again, so that there are no repetition of configurations	
	for( vector<Variant>::iterator iter = algorithm.population.begin(); iter != algorithm.population.end(); ++iter) {		
		map<TuningPoint*,int> vectorMap = (*iter).getValue();
		string uniqueElem = toString(*iter,algorithm.tuningPointVec);
		algorithm.populElem.insert(uniqueElem);
	}
	
	// Priting evaluation of this generation, later this code can be removed
	algorithm.logString.append("Objective values after generation:\n {" );
	for(vector<Variant>::iterator iter= algorithm.population.begin();iter!= algorithm.population.end(); ++iter) {
		
		vector<double> objVal = evalObjFunction(*iter, algorithm);
//		double objVal = evaluate(*iter, algorithm);
		
		algorithm.logString.append("(");
		for(size_t i=0; i<objVal.size(); i++) {
			stringstream ss1;
			ss1 << objVal[i];
//			ss1 << objVal;
			algorithm.logString.append( ss1.str() + ",");
		}
//		algorithm.logString.erase(algorithm.logString.find_last_of(','),1);
		algorithm.logString.append(")");
	}
	
	algorithm.logString.append("}\n");
}

void checkSearchFinished( Gde3Algorithm & algorithm ) {
	if(samePopulationVector == 3) {
		algorithm.setSearchFinished(true);		
	} else {
		if(recentPopulation.empty()) { // first time
			vector<Variant>::iterator iter;
			for(iter=algorithm.population.begin(); iter != algorithm.population.end(); ++iter) {
				recentPopulation.push_back(*iter);
			}			
		}
		
		bool generationEqual = true;
		
		// compare two population vectors
		if(algorithm.population.size() == recentPopulation.size()) {
			for(std::size_t i=0; i< recentPopulation.size(); i++) {
				if( toString(algorithm.population[i],algorithm.tuningPointVec) != toString(recentPopulation[i],algorithm.tuningPointVec) ){
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
		for(iter=algorithm.population.begin(); iter != algorithm.population.end(); ++iter) {
			recentPopulation.push_back(*iter);
		}					
	}
}

TuningPoint * createTuningPoint(long id, string tuningActionName, tunableType parameterType, int minRange,
			  					int maxRange, Gde3Algorithm & algorithm) {
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
	
	stringstream ssTest;
	ssTest << "Tuning plugin create with id: " << id << " Name: " << tuningActionName << " Parameter type: " 
	<< parameterType << " TuningPointRange: " << minRange << " to " << maxRange << "\n";
	algorithm.logString.append(ssTest.str());
	
	return tuningPointNew;
}


int main(int argc, char * argv[]) {
	string Usage = "\n\n GDE3 Implementation Usage: \n ./multiobj [No of Generations]\n\n";
	
	int noGenerations = 0;
	if(argc < 2) {
		cout << Usage;
		return 0;
	} else {
		string noGen = argv[1];
		istringstream(noGen) >> noGenerations;
	}
	
	cout << "GDE3 Implementation" << endl;
	
	Gde3Algorithm algorithm;

	// Creating variantspace
	VariantSpace * varSpace = new VariantSpace();
		
	map<TuningPoint*, TuningPointConstraint> space;
	
	// Adding first parameters to the map
	TuningPoint * tuningPointOne = createTuningPoint(100, "X value", CODE_REGION, -5, 5, algorithm);
	TuningPointConstraint tuningPointOneConstraint;		
	space[tuningPointOne] = tuningPointOneConstraint;
	algorithm.tuningPointVec.push_back(tuningPointOne);	
	
	// Adding second parameter
	TuningPoint * tuningPointTwo = createTuningPoint(200, "Y value", CODE_REGION, -5, 5, algorithm);
	TuningPointConstraint tuningPointTwoConstraint;	
	space[tuningPointTwo] = tuningPointTwoConstraint;
	algorithm.tuningPointVec.push_back(tuningPointTwo);
	
	varSpace->setSpace(space);		
	algorithm.addSearchSpace( varSpace );
	
	int iGen = 0;
	
	// Iteration starts here
	for(iGen=0; iGen < noGenerations; iGen++) {
		cout << "IterationNo: " << iGen << endl;
		cout << algorithm.logString << endl;
		cout << "createScenarios:" << endl;
		algorithm.createScenarios();
		
		cout << "evaluatePopulation:" << endl;
		evaluatePopulation(algorithm); // Will be replaced by evaluate in Periscope
		
		cout << "checkSearchFinished:" << endl;
		checkSearchFinished(algorithm);
		
		cout << "isSearchFinished:" << endl;
		if(algorithm.isSearchFinished()) {
			break;
		}				
	}
	
	algorithm.getOptimum();
	
	// Printing logs
	cout << algorithm.logString;
	cout << "Converge after " << iGen << " Generations !" << endl; 
	return 0;
}

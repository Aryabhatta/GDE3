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

// Map for reducing the no of evaluations, global place to store eval values
std::map< string,vector<double> > evalVec;

string toString( Variant & variant, vector<TuningPoint*> & TuningPointVec );

std::vector<double> evalObjFunction(Variant & variant, Gde3Algorithm & algorithm) {
	
	vector<double> result;
	
	// List all Tuning Parameters
	map<TuningPoint*,int> vectormap = variant.getValue();
	vector<int> tuningParam;
	
	for(std::size_t i=0; i < algorithm.tuningPointVec.size(); i++) {
		int value = vectormap[algorithm.tuningPointVec[i]];
		tuningParam.push_back(value);
	}
	
	/* Computing Ackley's function (Objective function)
	 * f(x,y) = -20 exp(-0.2 * Sqrt(0.5*(x^2+y^2))) - exp(0.5 * (cos(2*pi*x) + cos(2*pi*y))) + 20 + e 
	 * 
	 */
	double objFunc1 = -20 * exp(-0.2 * (sqrt(0.5 * (tuningParam[0]*tuningParam[0]+tuningParam[1]*tuningParam[1])))) 
	                  - exp(0.5 * (cos(2*M_PI*tuningParam[0]) + cos(2*M_PI*tuningParam[1]))) + 20 + M_E;
	
	result.push_back(objFunc1);	
	return result;
}

void evalPopulation(Gde3Algorithm & algorithm) { // fills the eval vector for new
	
	algorithm.logString.append("\n Evaluation: \n {" );
	
	// Imitating Periscope Evaluation
	for(std::size_t i=0; i<algorithm.population.size(); i++) {
		if(evalVec.count(toString(algorithm.population[i], algorithm.tuningPointVec)) != 1) {
			vector<double> objVal =	evalObjFunction(algorithm.population[i], algorithm);
			evalVec[toString(algorithm.population[i], algorithm.tuningPointVec)] = objVal;
			
			algorithm.logString.append("(");
			for(int k=0; k<objVal.size(); k++) {
				stringstream ss;
				ss << objVal[k];
				algorithm.logString.append(ss.str());
				algorithm.logString.append(",");
				
				algorithm.optimalObjVal = std::min(objVal[k],algorithm.optimalObjVal);		
				
			}
			algorithm.logString.append(") ");
		}
	}

	algorithm.logString.append(" }\n\n");
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

	// Creating new searchSpace 
	SearchSpace * searchSpace = new SearchSpace();
	
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
	searchSpace->setVariantSpace(varSpace);
	
	algorithm.addSearchSpace(searchSpace);
	
	int iGen = 0;
	
	// Iteration starts here
	for(iGen=0; iGen < noGenerations; iGen++) {
		algorithm.createScenarios();
		
		evalPopulation(algorithm); // Will be replaced by evaluate in Periscope
		
		algorithm.checkSearchFinished( evalVec );
		
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

//============================================================================
// Name        : GDE3.cpp
// Author      : ShrikantV
// Version     :
// Copyright   : Your copyright notice
// Description : GDE3 Implementation
//============================================================================

#include <iostream>
#include <sstream>
#include "searchalgorithms/GdeAlgorithm.h"
using namespace std;

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
	
	GdeAlgorithm algorithm;
	
	VariantSpace * variantSpace = algorithm.initialize();
	
	algorithm.addSearchSpace( variantSpace );
	
	// loop starts here
	for(int i=0; i<noGenerations; i++) {
		algorithm.createScenarios();
		
		algorithm.doSearch();
		
		algorithm.getOptimum();
		
		if(algorithm.isSearchFinished()) {
			break;
		}
	}
	
	return 0;
}

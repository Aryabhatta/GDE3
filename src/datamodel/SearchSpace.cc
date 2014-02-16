#include "SearchSpace.h"

//void SearchSpace::addRegion(Region* reg) {
//	regions.push_back(reg);
//}
//
//// TODO check if to add an iterator instead
//vector<Region*> SearchSpace::getRegions() {
//	return regions;
//}

void SearchSpace::setVariantSpace(VariantSpace* vs){
	variantSpace=vs;
}


VariantSpace* SearchSpace::getVariantSpace(){
	return variantSpace;
}



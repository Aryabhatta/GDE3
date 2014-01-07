#ifndef VARIANTSPACE_H_
#define VARIANTSPACE_H_

#include <map>
#include "TuningPoint.h"
#include "TuningPointConstraint.h"
using namespace std;

class VariantSpace
{
	map<TuningPoint*, TuningPointConstraint > space; //subset of TS
public:
	VariantSpace();
	virtual ~VariantSpace();
	
	// return the space
	map<TuningPoint*, TuningPointConstraint> getSpace() const {
		return space;
	}

	// set the space
	void setSpace(map<TuningPoint*, TuningPointConstraint> space) {
		this->space = space;
	}
};

#endif /*VARIANTSPACE_H_*/

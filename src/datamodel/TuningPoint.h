#ifndef TUNINGPOINT_H_
#define TUNINGPOINT_H_

#include <string>
#include <iostream>

#include "TuningPointRange.h"
#include "Restriction.h"

using namespace std;

enum tunableType {
	UNKOWN_TYPE, COMPILER_FLAGS, CODE_REGION, VARIABLE, FUNCTION_POINTER
};

class TuningPoint
{
	long ID;
	tunableType parameterType;
	string tuningActionName; //Selector1
	TuningPointRange tpRange;
	Restriction restriction; //e.g. Region
public:
	TuningPoint();
	virtual ~TuningPoint();
	
	long getId() const;
	void setId(long id);

	tunableType getParameterType() const;
	void setParameterType(tunableType parameterType);

	Restriction getRestriction() const;
	void setRestriction(Restriction restriction);

	string getTuningActionName() const;
	void setTuningActionName(string tuningActionName);

	TuningPointRange getTpRange() const;
	void setTpRange(TuningPointRange tpRange);

	string toString();
};

#endif /*TUNINGPOINT_H_*/

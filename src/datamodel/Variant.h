#ifndef VARIANT_H_
#define VARIANT_H_

#include <map>
#include "TuningPoint.h"
using namespace std;

class Variant
{
	map<TuningPoint*, int> value;
public:
	Variant();
	Variant(map<TuningPoint*,int> value){setValue(value);}
	virtual ~Variant();
	map<TuningPoint*,int> getValue() const {return value;}
    void setValue(map<TuningPoint*,int> value) {this->value = value;}
};

#endif /*VARIANT_H_*/

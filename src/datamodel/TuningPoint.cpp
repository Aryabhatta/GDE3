#include "TuningPoint.h"

TuningPoint::TuningPoint()
{
}

TuningPoint::~TuningPoint()
{
}

long TuningPoint::getId() const {
	return ID;
}

void TuningPoint::setId(long id) {
	this->ID = id;
}

tunableType TuningPoint::getParameterType() const {
	return parameterType;
}

void TuningPoint::setParameterType(tunableType parameterType) {
	this->parameterType = parameterType;
}

TuningPointRange TuningPoint::getTpRange() const {
	return this->tpRange;
}

void TuningPoint::setTpRange(TuningPointRange tpRange) {
	this->tpRange = tpRange;
}

Restriction TuningPoint::getRestriction() const{
	return restriction;
}

void TuningPoint::setRestriction(Restriction restriction){
	this->restriction = restriction;
}


string TuningPoint::toString(){
	cout << "TuningPoint details:" << endl;
	cout << "ID:             " << this->getId() << endl;
//	cout << "Plugin type:    " << this->getPluginType() << endl;
	cout << "Parameter type: " << this->getParameterType() << endl;
	cout << "Action name:    " << this->getTuningActionName() << endl;
	cout << "Range:          ";
//	BOOST_FOREACH(int i, this->getTpRange().getRange()) {
//		std::cout << i << ' ';
//	}
	cout << endl;
}

string TuningPoint::getTuningActionName() const {
	return this->tuningActionName;
}

void TuningPoint::setTuningActionName(string tuningActionName) {
	this->tuningActionName = tuningActionName;
}

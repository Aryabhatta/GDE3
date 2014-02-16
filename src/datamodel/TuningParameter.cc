/**
 @file	TuningParameter.cpp
 @ingroup Autotune
 @brief   Tuning Parameter
 @author	Houssam Haitof
 @verbatim
 Revision:       $Revision$
 Revision date:  $Date$
 Committed by:   $Author$

 This file is part of the Periscope performance measurement tool.
 See http://www.lrr.in.tum.de/periscope for details.

 Copyright (c) 2005-2012, Technische Universitaet Muenchen, Germany
 See the COPYING file in the base directory of the package for details.
 @endverbatim
 */

#include "TuningParameter.h"
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <stdio.h>

using namespace std;


TuningParameter::TuningParameter() {
	this->restriction=NULL;
}

TuningParameter::~TuningParameter() {

}

long TuningParameter::getId() const {
	return this->ID;
}

void TuningParameter::setId(long id) {
	this->ID = id;
}

//void TuningParameter::setRuntimeActionType(runtimeTuningActionType actionType) {
//	this->runtimeActionType = actionType;
//}
//
//runtimeTuningActionType TuningParameter::getRuntimeActionType() const {
//	return runtimeActionType;
//}

void TuningParameter::setPluginType(tPlugin pluginType) {
	this->pluginType = pluginType;
}

tPlugin TuningParameter::getPluginType() const {
	return pluginType;
}

int TuningParameter::getRangeFrom() const {
	return this->from;
}

int TuningParameter::getRangeTo() const {
	return this->to;
}

int TuningParameter::getRangeStep() const {
	return this->step;
}

void TuningParameter::setRange(int start, int stop, int incr) {
	this->from=start;
	this->to=stop;
	this->step=incr;
}

Restriction* TuningParameter::getRestriction() const{
	return restriction;
}

void TuningParameter::setRestriction(Restriction *restriction){
	this->restriction = restriction;
}


string TuningParameter::toString(){
	string ret_value;
	cout << "TuningParameter details:" << endl;
	cout << "ID:             " << this->getId() << endl;
	cout << "Plugin type:    " << this->getPluginType() << endl;
//	cout << "Parameter type: " << this->getRuntimeActionType() << endl;
	cout << "Action name:    " << this->getName() << endl;
	cout << "Range:          " << "(" << this->from << "," << this->to << "," << this->step << ")" << endl;
	cout << endl;
	return ret_value;
}

string TuningParameter::getName() const {
	return this->name;
}

void TuningParameter::setName(string name) {
	this->name = name;
}

bool TuningParameter::operator==(const TuningParameter &in) const {
	if( 
			ID != in.ID 
			|| pluginType != in.pluginType
//			|| runtimeActionType != in.runtimeActionType
			|| name.compare(in.name) != 0
			|| from != in.from
			|| to != in.to
			|| step != in.step
			|| *restriction != *in.restriction
			){
		return false;
	}
	return true;
}

bool TuningParameter::operator!=(const TuningParameter &in) const {
	return !(*this == in);
}

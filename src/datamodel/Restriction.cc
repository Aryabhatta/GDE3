/**
 @file	Restriction.cc
 @ingroup Autotune
 @brief   Tuning Parameter Restrictions
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


#include "Restriction.h"

Restriction::Restriction() {
	this->regionDefined=false;
//	this->region=NULL;
	type=1;
}

Restriction::~Restriction() {
}

bool Restriction::isRegionDefined() const {
    return regionDefined;
}

void Restriction::setRegionDefined(bool regionDefined) {
    this->regionDefined = regionDefined;
}

//Region* Restriction::getRegion() {
//	return region;
//}

//void Restriction::setRegion(Region* region) {
//	this->region = region;
//}


bool Restriction::operator==(const Restriction &in) const {

	if( 
			regionDefined != in.regionDefined 
//			|| *region != *in.region
		){
		return false;
	}
	return true;
}

bool Restriction::operator!=(const Restriction &in) const {
	return !(*this == in);
}

/**
   @file	SEARCHSPACE.h
   @ingroup Autotune
   @brief   Variant Space
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

#ifndef SEARCHSPACE_H_
#define SEARCHSPACE_H_

#include <list>
#include <map>
#include <vector>

//#include "application.h"
#include "VariantSpace.h"

using namespace std;


class SearchSpace {

//	vector<Region*> regions;
	// TODO check if getter/setters needed for vsConstraints
	VariantSpace* variantSpace;

public:
	SearchSpace(){}
	virtual ~SearchSpace(){}

//	void addRegion(Region* reg);
//	vector<Region*> getRegions();

	void setVariantSpace(VariantSpace* vs);
	VariantSpace* getVariantSpace();


};

#endif /* SEARCHSPACE_H_ */

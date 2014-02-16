/**
 @file	TuningParamter.h
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

#ifndef TUNINGPARAMETER_H_
#define TUNINGPARAMETER_H_

#include <list>
#include <vector>
#include <utility>
#include <string>

//#include "application.h"
#include "Restriction.h"

using namespace std;

enum tPlugin {
	UNKOWN_PLUGIN, CFS, HMPP, MPI, Power, Pipeline, PCAP
};

class TuningParameter {
//	friend class boost::serialization::access;
//	template<class Archive>
//		void serialize(Archive & ar, const unsigned int version){
//			ar & ID;
//			ar & pluginType;
//			ar & runtimeActionType;
//			ar & name;
//			ar & from;
//			ar & to;
//			ar & step;
//			ar & restriction;
//		}

	long ID;
	tPlugin pluginType;
//	runtimeTuningActionType runtimeActionType;
	string name; //Selector1
	int from, to, step;
	Restriction *restriction; //e.g. Region

public:
	TuningParameter();
	virtual ~TuningParameter();

	long getId() const;
	void setId(long id);

//	runtimeTuningActionType getRuntimeActionType() const;
//	void setRuntimeActionType(runtimeTuningActionType actionType);

	tPlugin getPluginType() const;
	void setPluginType(tPlugin pluginType);

	Restriction *getRestriction() const;
	void setRestriction(Restriction *restriction);

	string getName() const;
	void setName(string name);

	int getRangeFrom() const;
	int getRangeTo() const;
	int getRangeStep() const;
	void setRange(int start, int stop, int incr);

	string toString();

	bool operator==(const TuningParameter &in) const;
	bool operator!=(const TuningParameter &in) const;
};

#endif /* TUNINGPARAMETER_H_ */


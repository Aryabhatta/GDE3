#ifndef TUNINGPOINTRANGE_H_
#define TUNINGPOINTRANGE_H_

//#include <boost/iterator/counting_iterator.hpp>
//#include <boost/range/iterator_range.hpp>
//#include <boost/foreach.hpp>
//#include <boost/iterator/iterator_facade.hpp>
//#include <cassert>

class TuningPointRange
{
	//TODO: Avoiding boost syntaxes for time being, replace them afterwards
	int min,max;
	
public:
	TuningPointRange();
	TuningPointRange(int min, int max);
	virtual ~TuningPointRange();
	
	// newly added function
	int getMin() { return min; }
};

#endif /*TUNINGPOINTRANGE_H_*/

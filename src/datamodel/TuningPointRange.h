#ifndef TUNINGPOINTRANGE_H_
#define TUNINGPOINTRANGE_H_

#include <iostream>
//#include <boost/iterator/counting_iterator.hpp>
//#include <boost/range/iterator_range.hpp>
//#include <boost/foreach.hpp>
//#include <boost/iterator/iterator_facade.hpp>
//#include <cassert>
using namespace std;

class TuningPointRange
{
//	boost::iterator_range<boost::counting_iterator<int> > range;
	int min, max;
	
public:
	TuningPointRange();
	TuningPointRange(int min, int max);
	virtual ~TuningPointRange();
	
	// newly added function
	int getMin() const { return min; }
	int getMax() const { return max; } 
	
//	boost::iterator_range<boost::counting_iterator<int> > getRange() const {
//		return range;
//	}
//
//	void setRange(boost::iterator_range<boost::counting_iterator<int> > range) {
//		this->range = range;
//	}
};

#endif /*TUNINGPOINTRANGE_H_*/

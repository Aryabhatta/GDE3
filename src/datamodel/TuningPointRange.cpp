#include "TuningPointRange.h"

TuningPointRange::TuningPointRange()
{
}

TuningPointRange::~TuningPointRange()
{
}


TuningPointRange::TuningPointRange(int min, int max) {
	this->min = min;
	this->max = max;
}

//template <class T>
//boost::iterator_range<boost::counting_iterator<T> > tpRange(T max)
//{
//    assert(T() <= max);
//    return boost::make_iterator_range(boost::counting_iterator<T>(0), boost::counting_iterator<T>(max));
//}
//
//template <class T>
//boost::iterator_range<boost::counting_iterator<T> > tpRange(T min, T max)
//{
//    assert(min <= max);
//    return boost::make_iterator_range(boost::counting_iterator<T>(min), boost::counting_iterator<T>(max));
//}
//
//TuningPointRange::TuningPointRange() {
//}
//
//TuningPointRange::TuningPointRange(int min, int max) {
//	setRange(tpRange(min, max));
//}
//
///*TuningPointRange::TuningPointRange(int min, int max, int step) {
//	setRange(tpRange(min, max, step));
//}*/
//
//TuningPointRange::~TuningPointRange() {
//	// TODO Auto-generated destructor stub
//}

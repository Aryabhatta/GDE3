#ifndef CONSTRAINT_H_
#define CONSTRAINT_H_

#include <list>

using namespace std;

class Constraint
{
	int leftBoundary;
	int rightBoundary;
	list<int> elements;
public:
	bool assertValue(int);
	
	Constraint();
	virtual ~Constraint();
	
    list<int> getElements();
    int getLeftBoundary();
    int getRightBoundary();
    void setElements(list<int> elements);
    void setLeftBoundary(int leftBoundary);
    void setRightBoundary(int rightBoundary);

};

#endif /*CONSTRAINT_H_*/

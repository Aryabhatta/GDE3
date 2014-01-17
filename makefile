all: multiobj clean

# which compiler
CC = g++

# Options while compiling
CFLAGS =

# Options for CFITSIO
CFITSIO = 

# Options for FFTW
FFTW = -lfftw3 -lfftw3_omp

multiobj: driver.o Gde3Algorithm.o VariantSpace.o Variant.o TuningPoint.o TuningPointRange.o TuningPointConstraint.o Constraint.o Restriction.o
	$(CC) -o multiobj driver.o Gde3Algorithm.o VariantSpace.o Variant.o TuningPoint.o TuningPointRange.o TuningPointConstraint.o Constraint.o Restriction.o

driver.o: ./src/driver.cpp
	$(CC) $(CFLAGS) -c ./src/driver.cpp
        
Gde3Algorithm.o: ./src/searchalgorithms/Gde3Algorithm.cpp ./src/searchalgorithms/Gde3Algorithm.h
	$(CC) $(CFLAGS) -c ./src/searchalgorithms/Gde3Algorithm.cpp

VariantSpace.o: ./src/datamodel/VariantSpace.cpp ./src/datamodel/VariantSpace.h
	$(CC) $(CFLAGS) -c ./src/datamodel/VariantSpace.cpp

Variant.o: ./src/datamodel/Variant.cpp ./src/datamodel/Variant.h
	$(CC) $(CFLAGS) -c ./src/datamodel/Variant.cpp

TuningPoint.o: ./src/datamodel/TuningPoint.cpp ./src/datamodel/TuningPoint.h
	$(CC) $(CFLAGS) -c ./src/datamodel/TuningPoint.cpp

TuningPointRange.o: ./src/datamodel/TuningPointRange.cpp ./src/datamodel/TuningPointRange.h
	$(CC) $(CFLAGS) -c ./src/datamodel/TuningPointRange.cpp

TuningPointConstraint.o: ./src/datamodel/TuningPointConstraint.cpp ./src/datamodel/TuningPointConstraint.h
	$(CC) $(CFLAGS) -c ./src/datamodel/TuningPointConstraint.cpp

Constraint.o: ./src/datamodel/Constraint.cpp ./src/datamodel/Constraint.h
	$(CC) $(CFLAGS) -c ./src/datamodel/Constraint.cpp

Restriction.o: ./src/datamodel/Restriction.cpp ./src/datamodel/Restriction.h
	$(CC) $(CFLAGS) -c ./src/datamodel/Restriction.cpp

# Cleaning the intermediates
clean:
	rm -f *.o
	rm -f *~
	rm -f *.gch

# Cleaning all the intermediates
cleanAll:
	rm -f *.o
	rm -f *~
	rm -f *.gch

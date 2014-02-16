#include "Variant.h"

Variant::Variant(){ } 

Variant::Variant(map<TuningParameter*,int> value){
	setValue(value);
}

map<TuningParameter*,int> Variant::getValue() const {
	return value;
}

void Variant::setValue(map<TuningParameter*,int> value) {
	this->value = value;
}

bool Variant::operator==(const Variant &in) const{
	if( value != in.value ){
		cout << "false in variant check\n";
		return false;
	}

	cout << "true in variant check\n";
	return true;
}
bool Variant::operator!=(const Variant &in) const{
	return !(*this == in);
}

#include "Gate.h"
#include "Wire.h"
#include "Event.h"
//Written by Caleb Willson and Stephen Pearson
Gate::Gate(gates type, int delay, Wire* i1, Wire* i2, Wire* o)
{
	gateType = type;
	gateDelay = delay;
	in1 = i1;
	in2 = i2;
	out = o;
}

Gate::~Gate()
{
	delete in1;
	delete in2;
	delete out;
}

Wire* Gate::GetInput(int num) const
{
	if (num == 1) {
		return in1;
	}
	else if (num == 2) {
		return in2;
	}
	else {
		return nullptr;
	}
}

// Depending on the type of gate, test the logic
// Then check if it changed the output wire from its last known value
// Return a new Event whether the output changed or not.
//		If it didn't change it will return a "NULL" event with a time of -1 that will be ignored
//		If it did change it will return the appropriate event
Event Gate::evaluate(int t, int o)
{
	Event gateEvent(-1, -1, 0, 0);
	
	switch (gateType) {
	case AND:
		if (in1->GetValue() == 1 && in2->GetValue() == 1) {
			gateEvent = IsChange(1, t, o);
		}
		else if (in1->GetValue() == 0 || in2->GetValue() == 0) {
			gateEvent = IsChange(0, t, o);
		}
		else {
			gateEvent = IsChange(-1, t, o);
		}
		break;
	case NAND:
		if (in1->GetValue() == 1 && in2->GetValue() == 1) {
			gateEvent = IsChange(0, t, o);
		}
		else if (in1->GetValue() == 0 || in2->GetValue() == 0) {
			gateEvent = IsChange(1, t, o);
		}
		else {
			gateEvent = IsChange(-1, t, o);
		}
		break;
	case OR:
		if (in1->GetValue() == 1 || in2->GetValue() == 1) {
			gateEvent = IsChange(1, t, o);
		}
		else if (in1->GetValue() == 0 && in2->GetValue() == 0) {
			gateEvent = IsChange(0, t, o);
		}
		else {
			gateEvent = IsChange(-1, t, o);
		}
		break;
	case NOR:
		if (in1->GetValue() == 1 || in2->GetValue() == 1) {
			gateEvent = IsChange(0, t, o);
		}
		else if (in1->GetValue() == 0 && in2->GetValue() == 0) {
			gateEvent = IsChange(1, t, o);
		}
		else {
			gateEvent = IsChange(-1, t, o);
		}
		break;
	case XOR:
		if ((in1->GetValue() == 1 && in2->GetValue() == 1) || (in1->GetValue() == 0 && in2->GetValue() == 0)) {
			gateEvent = IsChange(0, t, o);
		}
		else if (in1->GetValue() == -1 || in2->GetValue() == -1) {
			gateEvent = IsChange(-1, t, o);
		}
		else {
			gateEvent = IsChange(1, t, o);
		}
		break;
	case XNOR:
		if ((in1->GetValue() == 0 && in2->GetValue() == 0) || (in1->GetValue() == 1 && in2->GetValue() == 1)) {
			gateEvent = IsChange(1, t, o);
		}
		else if (in1->GetValue() == -1 || in2->GetValue() == -1) {
			gateEvent = IsChange(-1, t, o);
		}
		else {
			gateEvent = IsChange(0, t, o);
		}
		break;
	case NOT:
		if (in1->GetValue() == 1) {
			gateEvent = IsChange(0, t, o);
		}
		else if (in1->GetValue() == -1) {
			gateEvent = IsChange(-1, t, o);
		}
		else {
			gateEvent = IsChange(1, t, o);
		}
		break;
	}
	return gateEvent;
}

// Check if the output changed and create the appropriate event
Event Gate::IsChange(int evalVal, int currTime, int order)
{
	if ((out->GetLastValue() != evalVal) && gateDelay + currTime <= 60) {
		return Event(out->GetIndex(), gateDelay + currTime, evalVal, order);
	}
	return Event(-1, -1, 0, 0);
}
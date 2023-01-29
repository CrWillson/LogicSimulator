#pragma once
#include "Event.h"

//Written by Caleb Willson and Stephen Pearson

using namespace std;

enum gates { AND, NAND, OR, NOR, XOR, XNOR, NOT, UNINIT };

class Wire;
class Event;
class Gate
{
public:
	// constructor
	Gate(gates type, int delay, Wire* i1, Wire* i2, Wire* o);
	~Gate();

	// Getter functions
	int getDelay() const { return gateDelay; }
	Wire* GetInput(int num) const;

	// the main update
	Event evaluate(int t, int o);

private:
	Event IsChange(int evalVal, int currTime, int order);
	gates gateType;
	int gateDelay;
	Wire* in1;
	Wire* in2;
	Wire* out;
};
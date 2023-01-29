#pragma once

//Written by Caleb Willson and Stephen Pearson

#include <vector>
#include <string>
#include <iostream>

using namespace std;

enum wires { INPUT, OUTPUT, INTERNAL };

class Event;
class Gate;

class Wire
{
public:
	// constructor
	Wire(int val, string n, int i, wires type);
	~Wire();

	// Wire modifiers
	vector<Event> SetValue(int val, int t, int o);
	void ReconHistory();
	void AddOutput(Gate* gate) { out.push_back(gate); }
	void AddHistory(int val, int time);
	void AddHistory(Event in);
	void PrintWire(int len);

	// Getter functions
	int GetValue() const { return value; };
	string GetName() const { return name; };
	int GetIndex() const { return index; };
	wires GetType() const { return wireType; }
	int GetHistLen() const { return history.size(); }
	int GetLastValue();

private:
	int value;
	string name;
	vector <Gate*> out;
	int index;
	vector<int> history = { -1 };
	vector<Event> eventHis;
	wires wireType;
};
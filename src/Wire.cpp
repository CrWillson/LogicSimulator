#include "Wire.h"
#include "Gate.h"
#include "Event.h"
//Written by Caleb Willson and Stephen Pearson
Wire::Wire(int val, string n, int i, wires type)
{
	value = val;
	name = n;
	index = i;
	wireType = type;
}

Wire::~Wire()
{
	for (int i = 0; i < out.size(); i++) {
		delete out.at(i);
	}
}

// Update the wire's value and add the value and time to it's event history
// Then evaluate any attached gates
// This returns a vector of events that the gate evaluations return
vector<Event> Wire::SetValue(int val, int t, int o)
{
	vector<Event> wireEventQ;
	
	value = val;
	AddHistory(val, t);

	for (int i = 0; i < out.size(); i++) {
		wireEventQ.push_back(out.at(i)->evaluate(t, o + wireEventQ.size()));
	}
	return wireEventQ;
}

// Reconstruct the full timeline from the event history
void Wire::ReconHistory()
{
	// first clean out any duplicate events by taking the "newest" event at each time period
	for (int i = 0; i < eventHis.size(); i++) {
		Event event1 = eventHis.at(i);
		for (int j = 0; j < eventHis.size(); j++) {
			Event event2 = eventHis.at(j);

			if ((event1.GetTime() == event2.GetTime()) && (i != j)) {
				if (event1.GetOoa() < event2.GetOoa()) {
					eventHis.erase(eventHis.begin() + i);
				}
				else {
					eventHis.erase(eventHis.begin() + j);
				}
			}
		}
	}
	
	// erase the initial "Null" event and reconstruct
	eventHis.erase(eventHis.begin());
	for (int i = 0; i < eventHis.size(); i++) {
		Event currEvent = eventHis.at(i);

		if (currEvent.GetTime() == 0) {
			history.at(0) = currEvent.GetValue();
		}
		else {
			while (history.size() < currEvent.GetTime()) {
				history.push_back(history.back());
			}
			history.push_back(currEvent.GetValue());
		}
	}
}

// Overloaded functions to add new events into the event history
void Wire::AddHistory(int val, int time)
{
	Event newEvent(index, time, val, 0);
	for (int i = 0; i < eventHis.size(); i++) {
		if (newEvent == eventHis.at(i)) {
			return;
		}
	}
	eventHis.push_back(newEvent);
}
void Wire::AddHistory(Event in)
{
	for (int i = 0; i < eventHis.size(); i++) {
		if (in == eventHis.at(i)) {
			return;
		}
	}
	eventHis.push_back(in);
}

// Fill the history up to a given length and print it
void Wire::PrintWire(int len)
{
	while (history.size() < len) {
		history.push_back(history.back());
	}

	if (wireType != INTERNAL) {
		cout << name << ": |";
		for (int i = 0; i < history.size(); i++) {
			if (history.at(i) == -1) { cout << "x"; }
			else if (history.at(i) == 0) { cout << "_"; }
			else if (history.at(i) == 1) { cout << "-"; }
			else { cout << history.at(i); }
		}
		cout << endl;
	}
}

// Get the last value of the wire from the event history
int Wire::GetLastValue()
{
	Event latestEvent(-1, -1, -1, 0);

	for (int i = 0; i < eventHis.size(); i++) {
		if (eventHis.at(i).GetTime() >= latestEvent.GetTime()) {
			latestEvent = eventHis.at(i);
		}
	}
	return latestEvent.GetValue();
}
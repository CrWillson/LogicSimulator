#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <queue>
#include "Wire.h"
#include "Gate.h"
#include "Event.h"

using namespace std;

Wire* getWirePtr(int id, vector<Wire*>& wires);
void ValStrToInt(string valStr, int& val);
void PrintTimer(int len, string name, string file, string vFile);

int main() {
	// Initial variable declaration and ask for file to parse
	ifstream in;
	string inputFile;
	string circuitName;

	priority_queue<Event> eventQ;

	vector<Wire*> wireVec;
	vector<Gate*> gateVec;

	cout << "Enter a file to simulate (type EXIT to quit): ";
	cin >> inputFile;
	
	while (inputFile != "EXIT") {

		string vectorFile = "../Circuits/" + inputFile + "_v.txt";
		inputFile = "../Circuits/" + inputFile + ".txt";

		// circuit parsing
		in.open(inputFile);

		if (!in.is_open()) {
			cerr << "Error opening circuit file" << endl;
			cerr << "Enter new circuit file to simulate (type EXIT to quit): ";
			cin >> inputFile;
			continue;
		}

		// all the variables the circuit parser needs
		string type, name, delayStr;
		int id, delay, i1, i2, out;
		gates gateType = UNINIT;

		// skips the first line and gets the circuit name
		in >> type;
		in >> circuitName;

		// parsing loop
		in >> type;
		while (!in.eof()) {
			// parse input wires
			if (type == "INPUT") {
				in >> name;
				in >> id;
				wireVec.push_back(new Wire(-1, name, id, INPUT));
				wireVec.back()->AddHistory(-1, -1);
			}
			// parse output wires
			else if (type == "OUTPUT") {
				in >> name;
				in >> id;
				wireVec.push_back(new Wire(-1, name, id, OUTPUT));
				wireVec.back()->AddHistory(-1, -1);
			}
			// handle NOT gates specially
			else if (type == "NOT") {
				in >> delayStr;
				sscanf_s(delayStr.c_str(), "%d", &delay);
				in >> i1;
				in >> out;
				Wire* in1Ptr = getWirePtr(i1, wireVec);
				Wire* outPtr = getWirePtr(out, wireVec);

				gateVec.push_back(new Gate(NOT, delay, in1Ptr, NULL, outPtr));
				in1Ptr->AddOutput(gateVec.back());
			}
			// get all the other gates
			else {
				in >> delayStr;
				sscanf_s(delayStr.c_str(), "%d", &delay);
				in >> i1;
				in >> i2;
				in >> out;
				Wire* in1Ptr = getWirePtr(i1, wireVec);
				Wire* in2Ptr = getWirePtr(i2, wireVec);
				Wire* outPtr = getWirePtr(out, wireVec);

				if (type == "AND") { gateType = AND; }
				else if (type == "NAND") { gateType = NAND; }
				else if (type == "OR") { gateType = OR; }
				else if (type == "NOR") { gateType = NOR; }
				else if (type == "XOR") { gateType = XOR; }
				else if (type == "XNOR") { gateType = XNOR; }

				gateVec.push_back(new Gate(gateType, delay, in1Ptr, in2Ptr, outPtr));
				in1Ptr->AddOutput(gateVec.back());
				in2Ptr->AddOutput(gateVec.back());
			}
			in >> type;
		}

		in.close();

		// Vector file parsing
		in.open(vectorFile);

		if (!in.is_open()) {
			cerr << "Error opening vector file" << endl;
			cerr << "Enter new circuit file to simulate (type EXIT to quit): ";
			cin >> inputFile;
			continue;
		}

		// more variables for the vecotr parsing
		int t = 0, order = 0, val = 0, index = -2;
		string valStr;

		// skip the first line
		in >> type;
		in >> type;

		in >> type;
		in >> name;
		in >> t;
		in >> valStr;

		// convert the wire name to its index number
		for (int i = 0; i < wireVec.size(); i++) {
			if (name == wireVec.at(i)->GetName()) {
				index = wireVec.at(i)->GetIndex();
				break;
			}
		}

		// this is just to convert "X" into -1 for the event handler
		ValStrToInt(valStr, val);

		// parsing loop
		while (!in.eof()) {
			eventQ.push(Event(index, t, val, order));

			in >> type;
			in >> name;
			in >> t;
			in >> valStr;
			order++;
			index = -2;
			for (int i = 0; i < wireVec.size(); i++) {
				if (name == wireVec.at(i)->GetName()) {
					index = wireVec.at(i)->GetIndex();
					break;
				}
			}

			ValStrToInt(valStr, val);

		}
		eventQ.push(Event(index, t, val, order));
		in.close();

		int currOrder = eventQ.size();

		// event handling
		while (!eventQ.empty()) {
			// Get the first event
			Event c = eventQ.top();

			// Vector to temporarily hold any new events the wires return
			vector<Event> wireEvents;

			// If the top event is a "NULL" event (time = -1) then pop it and restart the loop
			if (c.GetTime() == -1 || c.GetTime() > 60) {
				eventQ.pop();
				continue;
			}
			// If it gets a valid event, handle it
			else {
				// Find which wire the event modifies
				for (int i = 0; i < wireVec.size(); i++) {
					if (c.GetIndex() == wireVec.at(i)->GetIndex()) {
						// Set the wire's value and store any returned events in wireEvents
						wireEvents = wireVec.at(i)->SetValue(c.GetValue(), c.GetTime(), currOrder);
					}
				}
				// If there the event generated any new events, sort and store them
				for (int i = 0; i < wireEvents.size(); i++) {
					if (wireEvents.at(i).GetTime() != -1 && c.GetTime() <= 60) {
						// all the events go in the main Event Queue
						eventQ.push(wireEvents.at(i));
						if (wireEvents.at(i).GetOoa() >= currOrder) {
							currOrder = wireEvents.at(i).GetOoa() + 1;
						}
						// the events also get sorted into each wire's event history if it isn't in there already
						for (int j = 0; j < wireVec.size(); j++) {
							if (wireVec.at(j)->GetIndex() == wireEvents.at(i).GetIndex()) {
								wireVec.at(j)->AddHistory(wireEvents.at(i));
							}
						}
					}
				}
				// remove the event it just processed and loop
				eventQ.pop();
			}
		}

		// reconstruct the full timeline from each wire's event history
		// at the same time, find the length of the longest history
		int maxHist = 0;
		for (int i = 0; i < wireVec.size(); i++) {
			if (wireVec.at(i)->GetType() != INTERNAL) {
				wireVec.at(i)->ReconHistory();
			}
			if (wireVec.at(i)->GetHistLen() >= maxHist) {
				maxHist = wireVec.at(i)->GetHistLen();
			}
		}
		// fill any gaps at the end of wire traces so they are all the same length
		// this also prints the wire traces for any INPUT or OUTPUT wires
		cout << endl;
		for (int i = 0; i < wireVec.size(); i++) {
			wireVec.at(i)->PrintWire(maxHist);
		}
		// print the time line as well as file info under the wire traces
		PrintTimer(maxHist, circuitName, inputFile, vectorFile);

		cout << "Enter a file to simulate (type EXIT to quit): ";
		cin >> inputFile;

		wireVec.clear();
		gateVec.clear();
	}
	return 0;
}

// function to attach gates to wires and create new wires if one wasn't declared by the user
Wire* getWirePtr(int id, vector<Wire*>& wires)
{
	for (int i = 0; i < wires.size(); i++) {
		if (id == wires.at(i)->GetIndex()) {
			return wires.at(i);
		}
	}
	wires.push_back(new Wire(-1, "INTERNAL", id, INTERNAL));
	wires.back()->AddHistory(-1, -1);
	return wires.back();
}

// Used by vector parser to convert any X's into -1's
void ValStrToInt(string valStr, int& val)
{
	if (valStr == "0") { val = 0; }
	else if (valStr == "1") { val = 1; }
	else if (valStr == "X" || valStr == "x") { val = -1; }
}

// Print the appropiate length of timeline
void PrintTimer(int len, string name, string file, string vfile)
{
	string timerL = "   |_____________________________________________________________";
	string timerT = "    0----0----1----1----2----2----3----3----4----4----5----5----6";
	string timerB = "    0    5    0    5    0    5    0    5    0    5    0    5    0";

	cout << timerL.substr(0, len + 4) << endl;
	cout << timerT.substr(0, len + 4) << endl;
	cout << timerB.substr(0, len + 4) << endl;
	cout << endl;
	cout << "Simulated " << name << " from " << file << endl;
	cout << "Using vector file: ";
	for (int i = 0; i < name.length() - 3; i++) { cout << " "; }
	cout << vfile << endl;
	cout << "Total time: " << len - 1 << "ns." << endl;
	if (len > 60) { cout << "Note: Simulation has reached maximum length. Some data may be cut off." << endl; }
	cout << endl;
}
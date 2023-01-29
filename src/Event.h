#pragma once
#include <string>
#include <iostream>

using namespace std;

class Event
{
public:
	// constructor and operators
	Event(int i, int t, int v, int o) : index(i), time(t), value(v), ooa(o) {};
	friend bool operator<(const Event& lhs, const Event& rhs);
	friend bool operator==(const Event& lhs, const Event& rhs);
	friend bool operator!=(const Event& lhs, const Event& rhs);

	// Getter functions
	int GetIndex() { return index; }
	int GetTime() { return time; }
	int GetValue() { return value; }
	int GetOoa() { return ooa; }

	// Setters and print functions
	void SetValue(int newVal) { value = newVal; }
	void SetTime(int newTime) { time = newTime; }
	void SetIndex(int newIndex) { index = newIndex; }
	void SetOrder(int newOrder) { ooa = newOrder; }
	void PrintEvent() const;
private:
	int index;
	int time;
	int value;
	int ooa;
};
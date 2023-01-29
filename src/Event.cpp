#include "Event.h"

bool operator<(const Event& lhs, const Event& rhs)
{
	if (lhs.time == rhs.time) {
		return lhs.ooa > rhs.ooa;
	}
	return lhs.time > rhs.time;
}

bool operator==(const Event& lhs, const Event& rhs)
{
	if ((lhs.time == rhs.time) && (lhs.index == rhs.index) && (lhs.value == rhs.value)) { return true; }
	else { return false; }
}

bool operator!=(const Event& lhs, const Event& rhs)
{
	if (lhs == rhs) { return false; }
	else { return true; }
}

// Unused in final program, primarily for debugging
void Event::PrintEvent() const
{
	cout << index << " at time " << time << " set to " << value << endl;
}
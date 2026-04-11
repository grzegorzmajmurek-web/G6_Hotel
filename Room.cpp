#include <string>
#include <iostream>
using namespace std;

class Room {

private:
	int number;
	int capacity;
	int type;
	int status;
public:
	Room(int n, int c, int t, bool s)
		: number(n), capacity(c), type(t), status(s) {
	}
	//gettery
	int getNumber() {
		return number;

	}

	int getCapacity() {
		return capacity;

	}
	int getType() {
		return type;

	}
	int getStatus() {
		return status;

	}
	//settery
	int setStatus(bool s) {
		status = s;
		return status;
	}

};

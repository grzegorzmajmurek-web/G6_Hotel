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
		cout << n << " " << c << " " << t << " " << s << endl;
	}
	//gettery
	int getNumber() {
		return n;

	}

	int getCapacity() {
		return c;

	}
	int getType() {
		return t;

	}
	int getStatus() {
		return s;

	}
	//settery
	int setStatus(bool status) {
		s = status;
		return s;
	}

};
int main() {
	Room(1, 10, 1, true);
	return 0;
}
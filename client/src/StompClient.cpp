#include <queue>
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include "../include/Frame.h"
using namespace std;


void KeyboardThread() {
	while(1) {
		const short bufsize = 1024;
		char buf[bufsize];
		cin.getline(buf, bufsize);
		string line(buf);
		std::cout << "input is:\n" + line << endl;
		(*departingMessages).push(line);
	}
}

int main(int argc, char *argv[]) {

	KeyboardThread();
	return 0;
}
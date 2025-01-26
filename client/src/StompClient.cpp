#include <queue>
#include <iostream>
#include <thread>
#include <mutex>
#include "../include/Frame.h"
#include <ConnectionHandler.h>
#include <vector>
#include "../include/StompProtocol.h"
using namespace std;



string HOST;
short PORT;

int main(int argc, char *argv[]) {
	try {
		if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
	cout << "REMEMBER TO REMOVE THE RUNNING COMMAND FROM THE MAKEFILE" << endl;
    HOST = argv[1];
    PORT = atoi(argv[2]);
	shared_ptr<StompProtocol> protocol = std::make_shared<StompProtocol>();
	string inputLine;
	while (1) {
		cout << "[DEBUG]: Please enter a command" << endl;
		getline(cin, inputLine);
		protocol->createDepartingFrame(inputLine);
	}
	}
	catch (exception& e) {
		cout << "Exception: " << e.what() << endl;
		return 1;
	}
	return 0;
}
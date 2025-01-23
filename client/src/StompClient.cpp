#include <queue>
#include <iostream>
#include <thread>
#include <mutex>
#include "../include/Frame.h"
#include <ConnectionHandler.h>
#include <vector>
#include "../include/StompProtocol.h"
using namespace std;



void InputThread() {
	while(1) {
		const short bufsize = 1024;
		char buf[bufsize];
		cin.getline(buf, bufsize);
		string line(buf);
		std::cout << "[DEBUG]: input is:\n" + line << endl;


		if (!connectionHandler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
	}
}

void OutputThread() {
	while(1) {
		string answer;
		if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        
		int len=answer.length();
        answer.resize(len-1);
        std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
	}
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
	shared_ptr<StompProtocol> protocol = std::make_shared<StompProtocol>();
	//initialize input and output threads
	
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

	thread inputThread(InputThread, );
	thread outputTHread(OutputThread, );

}
class ThreadsHandler {
public:
    
	
    void foo()
    {
        cout << "Thread using non-static member function "
                "as callable"
             << endl;
    }
    // static member function
    static void foo1()
    {
        cout << "Thread using static member function as "
                "callable"
             << endl;
    }
};



int main(int argc, char *argv[]) {
	// TODO: implement the STOMP client
	return 0;
}
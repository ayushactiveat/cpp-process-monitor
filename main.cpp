#include <iostream> // input/output like cout
#include <fstream> // to read files from /pric direct
#include <string> // string class
#include <unistd.h> // for getpid()

using namespace std;

//helper function to read a file from virtual /proc filesystem
string readProcFile(const string& path){
	ifstream file(path); // ifstream opens file for reading

	if(!file.is_open()) return "Error: Could not open file."; //handle race conditions i.e process dies before we read it

	string content; //empty string to hold file's content
	getline(file, content); // read one line from file and store in content
	file.close(); //close the file

	return content;
}

int main() {}

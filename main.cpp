#include <iostream> // input/output like cout
#include <fstream> // to read files from /pric direct
#include <string> // string class
#include <unistd.h> // for getpid()
#include <filesystem> // c++17 feture to loop through direct

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

int main() {
	for(const auto& entry : filesystem::directory_iterator("/proc")){ //loop throuh every file/folder in /proc direc
		string pid_str = entry.path().filename().string(); // get the filename

		if(pid_str.find_first_not_of("0123456789") == string::npos){ // check if name is a number

			// read process name from /proc/[PID]/comm , (pid = process ID)
			string procPath = "/proc/" + pid_str + "/comm";
			string procName = readProcFile(procPath);

			// if read was successful, print pid and name
			if(!procName.empty()){
				cout << "PID :" << pid_str << " | Name: " << procName << endl;
			}
		}
	}
	return 0;
}

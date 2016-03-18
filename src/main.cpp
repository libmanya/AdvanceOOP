#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <errno.h>
#include <vector>
using namespace std;

int main(int argc, char **argv)
{
	string configPath = "config.ini";
	//Check argument number
	if (argc > 2)
	{
		cout << "Only Two arugments allowed" << endl;
	}
	else
	{
		//Init Simulator
	}

	return 0;
}

int getDirContent(vector<string> &files)
{
	//DIR *dp;
	//struct dirent *dirp;
	//if ((dp = opendir(dir.c_str())) == NULL) {
	//	cout << "Error(" << errno << ") opening " << dir << endl;
	//	return errno;
	//}

	//while ((dirp = readdir(dp)) != NULL) {
	//	files.push_back(string(dirp->d_name));
	//}
	//closedir(dp);
	return 0;
}


#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string>
int main(int argc, char** argv)
{
	long size;
	char* buf;
	char* ptr;

	size = pathconf(".", _PC_PATH_MAX);


	if ((buf = (char *)malloc((size_t)size)) != NULL)
    		ptr = getcwd(buf, (size_t)size);
	
	std::string path(buf);
	std::cout << "Path: "  << path << std::endl;

	return 0;
}

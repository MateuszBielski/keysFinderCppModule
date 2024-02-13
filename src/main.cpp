#include <stdio.h>
#include "edgeprocessing.h"
#include "ini/ini.h"

int main(int argc, char **argv)
{
	inih::INIReader r{"../config.ini"};
    const auto& imagePath = r.Get<std::string>("section1", "imagePath");
    printf("hello world\n");
    EdgeProcessing ep;
    ep.LoadImageBW(imagePath);
	return 0;
}

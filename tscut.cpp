#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <stdlib.h> // atoi()

#define VERSION "1.0"

void Usage(char *pName) {
    std::cout << "NAME" << std::endl;
    std::cout << "   " << pName << " - cut mpeg ts content" << std::endl;
    std::cout << "   Version " << VERSION << std::endl;
    std::cout << std::endl;
    std::cout << "SYNOPSIS" << std::endl;
    std::cout << "   " << pName << " FILE [-split <nb packet>]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION" << std::endl;
    std::cout << "   cut ts stream" << std::endl;
    std::cout << std::endl;
    std::cout << "    -split <nb packets>" << std::endl;
    std::cout << "          cut file every nb packets" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    // help
    if (argc == 1)
    {
        Usage(argv[0]);
        return 0;
    }

    // check filename
    std::ifstream tsFile(argv[1], std::ios::binary);
    if (!tsFile.is_open()) {

        std::cerr << "Failed to open file " << argv[1] << std::endl;
        return -1;
    }
    else {

        // align file on 0x47
        char start[512];
        int index = 0;

        tsFile.read((char*)start, 512);
        while (start[index] != 0x47 && start[index+188] != 0x47 && (index+188) < 512) index++;

        tsFile.seekg(index);
    }

    // check options
    std::string StrSplit("-split");
    unsigned int splitNb = 0;

    for (int i=2; i<argc; ++i)
    {
        std::string StrOption(argv[i]);
        if (StrOption == StrSplit) splitNb = atoi(argv[++i]);
    }

    // display pid info
    if (splitNb){
        char* data = (char*)malloc(188*splitNb);
        int totalRead = 0;

        while (tsFile.read((char*)data, 188*splitNb))
        {
            std::ostringstream patch;
            patch << "_" << totalRead;

            std::string Filename(argv[1]);
            std::size_t found = Filename.find_last_of(".");

            Filename.insert(found, patch.str());

            std::ofstream tsOutFile(Filename.c_str(), std::ios::binary);
            tsOutFile.write(data, 188*splitNb);
            tsOutFile.close();
            totalRead += splitNb;

           std::cout << "Writing file " << Filename << std::endl;
        }
        free (data);
    }

    // close
    tsFile.close();

    return 0;
}

#include <iostream>
#include <fstream>
#include <string>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <fstream>
using std::ios;
using std::ifstream;

#include <exception>
using std::exception;

#include <cstring>
#include <cstdlib>
using std::exit;
using std::memcmp;

bool equalFiles(const std::string& file1, const std::string& file2);


//Doesn't work. Some issues in the EOF section
bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1), f2(file2);
    char ch1, ch2;

    while (f1.get(ch1) && f2.get(ch2)) {
        if (ch1 != ch2)
            return false;
    }

    std::cout<<"Reached here........";

    std::cout<<f1.eof()<<"  --->"<<f2.eof()<<"   ---------";

    // If both files reach EOF at the same time, they match
    if ((!f1.eof() && f2.eof()) || (f1.eof() && !f2.eof()))
    {
        return false;
    }   
    else
        return true;
}


bool equalFiles(const std::string& file1, const std::string& file2)
{
    std::ifstream in1(file1), in2(file2);
    ifstream::pos_type size1, size2;

    size1 = in1.seekg(0, ifstream::end).tellg();
    in1.seekg(0, ifstream::beg);

    size2 = in2.seekg(0, ifstream::end).tellg();
    in2.seekg(0, ifstream::beg);

    if(size1 != size2)
        return false;

    static const size_t BLOCKSIZE = 4096;
    size_t remaining = size1;

    while(remaining)
    {
        char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
        size_t size = std::min(BLOCKSIZE, remaining);

        in1.read(buffer1, size);
        in2.read(buffer2, size);

        if(0 != memcmp(buffer1, buffer2, size))
            return false;

        remaining -= size;
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <file1> <file2>" << std::endl;
        return 1;
    }

    std::string file1(argv[1]);
    std::string file2(argv[2]);

    std::cout<<"Comparing ............."<<file1<<"       .....and.... "<<file2<<std::endl;

    if (equalFiles(file1, file2)) {
        std::cout << "File Matches" << std::endl;
    } else {
        std::cout << "!!!!!!!!!!!!!!!!!Doesn't match" << std::endl;
    }

    return 0;
}

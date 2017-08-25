#include "Common.h"

#include "OpenCVStream.h"

int main(int argc, char* argv[])
{
    OpenCVStream* stream;

    stream = new OpenCVStream();
    stream->StreamON();

    std::cout << "Enter to leave!" << std::endl;

    getchar();

    stream->StreamOFF();

    return 0;
}

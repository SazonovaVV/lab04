#include <curl/curl.h>
#include "histogram.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

int
main(int argc, char* argv[])
{

    Input input;


    if (argc > 1)
    {

            input = download(argv[1]);

    }

    else
    {
        input = read_input(cin,true);
    }


    const auto bins = make_histogram(input);

    show_histogram_svg(bins);

    return 1;
}


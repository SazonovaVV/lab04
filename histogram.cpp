
#include "histogram.h"



void find_minmax(const vector<double>& numbers, double& min, double& max)
{
    min = numbers[0];
    max = numbers[0];

    for (size_t i = 0; i < numbers.size(); i++)
    {
        if (numbers[i] < min)
        {
            min = numbers[i];
        }
        if (numbers[i] > max)
        {
            max = numbers[i];
        }
    }

    return;
}
vector<double> input_numbers(istream& in, size_t count)
{
    vector<double> result(count);
    for (size_t i = 0; i < count; i++)
    {
        in >> result[i];
    }
    return result;
}

Input read_input(istream& in, bool prompt)
{

    Input data;
    if (prompt)
    {
        cerr << "Enter number count: ";
    }
    size_t number_count;
    in >> number_count;

    if(prompt)
    {
        cerr << "Enter numbers: ";
    }
    data.numbers = input_numbers(in, number_count);

    if(prompt)
    {
        cerr <<"Enter bin count:";
    }
    in>>data.bin_count;

    return data;

}
Input download(const string& address)
{
    stringstream buffer;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    if(curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, address.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        string namelookup;
        double t=curl_easy_getinfo(curl,CURLINFO_NAMELOOKUP_TIME_T,&namelookup);

        res = curl_easy_perform(curl);
        if(res)
        {
            cerr<< curl_easy_strerror(res)<<"("<<res<<")"<<" NAMELOOKUP:"<<t<<endl;
            exit(1);
        }
         else
         {
             cerr<<" NAMELOOKUP:"<<t<<endl;
         }
        curl_easy_cleanup(curl);
    }
    return read_input(buffer,false);
}
size_t write_data(void* items, size_t item_size, size_t item_count, void* ctx)
{
    size_t data_size = item_size * item_count;
    stringstream* buffer = reinterpret_cast<stringstream*>(ctx);
    buffer->write(reinterpret_cast<const char*>(items), data_size);
    return data_size;
}
string make_info_text()
{
    stringstream buffer;
    DWORD info = GetVersion();
    DWORD mask = 0x0000ffff;
    DWORD version = info & mask;
    DWORD platform = info >> 16;
    DWORD mask_2 = 0x0000ff;
    if ((info & 0x80000000) == 0)
    {
        DWORD version_major = version & mask_2;
        DWORD version_minor = version >> 8;
        DWORD build = platform;
        buffer << "Windows v"<<version_major<<"."<<version_minor<<"(build "<<build<<")"<<'\n';
//
    }
    char comp_name[MAX_COMPUTERNAME_LENGTH + 1];
    GetSystemDirectory(comp_name, MAX_PATH);
    printf("System directory: %s", comp_name);
    DWORD size = MAX_COMPUTERNAME_LENGTH+1;
    GetComputerNameA(comp_name, &size);
    buffer<<"Computer name:" <<comp_name;
    return buffer.str();
}
vector<double> make_histogram(Input data)
{

    double min, max;
    vector<double> bins(data.bin_count);
    find_minmax(data.numbers, min, max);
    size_t number_count = data.numbers.size();
    double bin_size = (max - min) / data.bin_count;
    for (size_t i = 0; i < number_count; i++)
    {
        bool flag = false;
        for (size_t j = 0; (j < data.bin_count - 1) && !flag; j++)
        {
            auto lo = min + j * bin_size;
            auto hi = min + (j + 1) * bin_size;
            if ((lo <= data.numbers[i]) && (hi > data.numbers[i]))
            {
                bins[j]++;
                flag = true;
            }

        }
        if (!flag)
        {
            bins[data.bin_count - 1]++;
        }
    }

    return bins;
}

void show_histogram_text(vector<size_t>bins)
{
    size_t bin_count = bins.size();
    size_t max_count = 0;
    for (size_t i = 0; i < bin_count; i++)
    {
        size_t count = bins[i];

        if (count > max_count)
        {
            max_count = count;
        }
    }
    double height;
    const size_t SCREEN_WIDTH = 80;
    const size_t MAX_ASTERISK = SCREEN_WIDTH - 3 - 1;
    if (max_count > MAX_ASTERISK)
    {
        height = (static_cast<double>(MAX_ASTERISK) / max_count);;
    }
    else
    {
        height = 1;
    }
    for (size_t i = 0; i < bin_count; i++)
    {
        size_t k = bins[i];
        cout << "   ";
        if (k < 100)
        {
            cout << " ";
        }
        if (k < 10)
        {
            cout << " ";
        }
        cout << k << "|";
        for (size_t j = 0; j < k * height; j++)
        {
            cout << "*";
        }
        cout << "\n";

    }
    return;
}
void
svg_begin(double width, double height)
{
    cout << "<?xml version='1.0' encoding='UTF-8'?>\n";
    cout << "<svg ";
    cout << "width='" << width << "' ";
    cout << "height='" << height << "' ";
    cout << "viewBox='0 0 " << width << " " << height << "' ";
    cout << "xmlns='http://www.w3.org/2000/svg'>\n";
}

void
svg_end()
{
    cout << "</svg>\n";
}
void
svg_text(double left, double baseline, string text)
{

    cout << "<text x='" << left << "' y='" << baseline << "'>" << text << "</text>";
}
void svg_rect(double x, double y, double width, double height, string stroke, string fill )
{
    cout << "<rect x='" << x << "' y='" << y << "' width='" << width << "' height='" << height
         << "' stroke='" << stroke << "' fill='" << fill << "'/>";
}

void show_histogram_svg( const vector<double>& bins)
{


    const auto IMAGE_WIDTH = 400;
    const auto IMAGE_HEIGHT = 300;
    const auto TEXT_LEFT = 20;
    const auto TEXT_BASELINE = 20;
    const auto TEXT_WIDTH = 50;
    const auto BIN_HEIGHT = 30;
    const auto BLOCK_WIDTH = 10;
    const auto MAX_COUNT = (IMAGE_WIDTH-TEXT_WIDTH)/ BLOCK_WIDTH;

    double top = 0;
    svg_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
    size_t count;
    size_t max_count = bins[0];
    for (size_t bin : bins)
    {
        if (bin > max_count)
        {
            max_count = bin;
        }
    }

    if (max_count > MAX_COUNT)
    {

        const double h = (double)MAX_COUNT / max_count;

        for (size_t bin : bins)
        {
            auto height = (size_t)(bin * h);
            const double bin_width = BLOCK_WIDTH * height;
            svg_text(TEXT_LEFT, top + TEXT_BASELINE, to_string(bin));
            svg_rect(TEXT_WIDTH, top, bin_width, BIN_HEIGHT, "black", "red");
            top += BIN_HEIGHT;

        }
        svg_text(0,top + TEXT_BASELINE, make_info_text());
        svg_end();
    }
    else
    {
        for (size_t bin : bins)
        {

            const double bin_width = BLOCK_WIDTH * bin;
            svg_text(TEXT_LEFT, top + TEXT_BASELINE, to_string(bin));
            svg_rect(TEXT_WIDTH, top, bin_width, BIN_HEIGHT, "black", "red");
            top += BIN_HEIGHT;

        }
        svg_text(0,top + TEXT_BASELINE, make_info_text());
        svg_end();
    }


}

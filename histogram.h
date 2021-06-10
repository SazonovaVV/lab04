#pragma once
#include <curl/curl.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <string>
#include <sstream>
#include<iomanip>
#include <fstream>


using namespace std;

struct Input
{
    vector<double> numbers;
    size_t bin_count;
};
Input read_input(istream& in,bool prompt);
Input download(const string& address);
size_t write_data(void* items, size_t item_size, size_t item_count, void* ctx);
void find_minmax(const vector<double>& numbers, double& min, double& max);
vector<double> input_numbers(istream& in, size_t count);
vector<double> make_histogram(Input data);
string make_info_text();
void show_histogram_text(vector<double>bins);
void svg_begin(double width, double height);
void svg_end();
void svg_text(double left, double baseline, string text);
void svg_rect(double x, double y, double width, double height, string stroke = "black", string fill = "blue");
void show_histogram_svg(const vector<double>& bins);



// includes
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

// declare an array of 3 bytes into the keyword "pixel"
typedef vector<uint8_t> pixel;

// function declarations
void readHeader(ifstream& file, int& rows, int& columns);
void readBody(ifstream& file, vector<pixel>& data, int rows, int columns);
void applyModification(const vector<pixel>& data, string filename , string modification, int rows, int columns);

void applyVerticalFlip(const vector<pixel>& data, ofstream& outfile, int rows, int columns);
void applyHorizontalFlip(const vector<pixel>& data, ofstream& outfile, int rows, int columns);
void applyHorizontalBlur(const vector<pixel>& data, ofstream& outfile, int rows, int columns);

uint8_t computeNegativeValue(uint8_t val);
uint8_t computeHighContrastValue(uint8_t val);
uint8_t computeRandomNoiseValue(uint8_t val);
uint8_t computeGrayScaleValue(pixel val);

// simple tool to remove the extention (.ppm) from a string
string removeExtention(string filename)
{
	std::string delimiter = ".";
	std::string token = filename.substr(0, filename.find(delimiter));

	return token;
}

int main()
{
	string input;

	cout << "Enter the input filename: ";
	cin >> input;

	// convert input to lowercase
	transform(input.begin(), input.end(), input.begin(), ::tolower);

	cin.ignore();

	ifstream file(input);

	vector<pixel> imageData;

	int rows = 0, columns = 0;

	readHeader(file, rows, columns);
	readBody(file, imageData, rows, columns);

	file.close();

	input = removeExtention(input);

	applyModification(imageData, input, "horizontal flip", rows, columns);
	applyModification(imageData, input, "vertical flip", rows, columns);
	applyModification(imageData, input, "negative", rows, columns);
	applyModification(imageData, input, "high contrast", rows, columns);
	applyModification(imageData, input, "remove red", rows, columns);
	applyModification(imageData, input, "remove green", rows, columns);
	applyModification(imageData, input, "remove blue", rows, columns);
	applyModification(imageData, input, "gray scale", rows, columns);
	applyModification(imageData, input, "random 50", rows, columns);
	
	cout << "Press Enter to Close";
	cin.ignore();

    return 0;
}

// reads the header of the input file
void readHeader(ifstream& file, int& rows, int& columns)
{
	string line;

	if ((int)file.tellg() == 0)
	{
		getline(file, line);

		cout << line << endl;

		if (line != "P3")
		{
			cout << "Invalid filetype!" << endl;
			return;
		}
	}

	file >> rows >> columns;
	
	cout << "Reading header: rows = " << rows << " columns = " << columns << endl;

	// ignore the next int as it isn't important to us
	int range;
	file >> range;
}

// reads the body of the image
void readBody(ifstream& file, vector<pixel>& data, int rows, int columns)
{
	cout << "reading body" << endl;

	// properly size the vector
	data.resize(rows * columns);

	// instead of defining these variable in each loop, define them before the loop starts
	int r, g, b;

	// fill in each portion of the data
	for (int i = 0; i < data.size(); i++)
	{
		file >> r >> g >> b;

		data[i].resize(3);

		data[i][0] = r;
		data[i][1] = g;
		data[i][2] = b;
	}

	cout << "finished reading body" << endl;
}

void applyModification(const vector<pixel>& data, string filename, string modification, int rows, int columns)
{
	string filename2(modification);

	replace(filename2.begin(), filename2.end(), ' ', '_');

	ofstream out(filename + "_" + filename2 + ".ppm");

	cout << "applying " << modification << " and writing to " << filename + "_" + filename2 + ".ppm" << endl;

	// write the header
	out << "P3" << endl << rows << " " << columns << endl << "255" << endl;

	if (modification == "vertical flip")
	{
		applyVerticalFlip(data, out, rows, columns);
	}
	else if (modification == "horizontal flip")
	{
		applyHorizontalFlip(data, out, rows, columns);
	}
	else if (modification == "horizontal blur")
	{
		applyHorizontalBlur(data, out, rows, columns);
	}
	else if (modification == "high contrast")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				out << 0 << (int)computeHighContrastValue(data[i][0]) << " " <<
					(int)computeHighContrastValue(data[i][1]) << " " << (int)computeHighContrastValue(data[i][2]) << (x == 0 ? "\n" : " ");
			}
		}
	}
	else if (modification == "negative")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				out << 0 << (int)computeNegativeValue(data[i][0]) << " " <<
					(int)computeNegativeValue(data[i][1]) << " " << (int)computeNegativeValue(data[i][2]) << (x == 0 ? "\n" : " ");
			}
		}
	}
	else if (modification == "random 50")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				out << 0 << (int)computeRandomNoiseValue(data[i][0]) << " " <<
					(int)computeRandomNoiseValue(data[i][1]) << " " << (int)computeRandomNoiseValue(data[i][2]) << (x == 0 ? "\n" : " ");
			}
		}
	}
	else if (modification == "gray scale")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				int val = computeGrayScaleValue(data[i]);

				out << 0 << val << " " << val << " " << val << (x == 0 ? "\n" : " ");
			}
		}
	}
	else if (modification == "remove red")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				out << 0 << " " << (int)data[i][1] << " " << (int)data[i][2] << (x == 0 ? "\n" : " ");
			}
		}
	}
	else if (modification == "remove green")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				out << (int)data[i][0] << " " << 0 << " " << (int)data[i][2] << (x == 0 ? "\n" : " ");
			}
		}
	}
	else if (modification == "remove blue")
	{
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				int i = x + (y * rows);

				out << (int)data[i][0] << " " << (int)data[i][1] << " " << 0 << (x == 0 ? "\n" : " ");
			}
		}	
	}

	cout << "finished applying " << modification << endl;

	out.close();
}

void applyHorizontalFlip(const vector<pixel>& data, ofstream& outfile, int rows, int columns)
{
	for (int y = 0; y < columns; y++)
	{
		for (int x = rows - 1; x >= 0; x--)
		{
			int i = x + (y * rows);

			outfile << (int)data[i][0] << " " << (int)data[i][1] << " " << (int)data[i][2] << (x == 0 ? "\n" : " ");
		}
	}
}

void applyVerticalFlip(const vector<pixel>& data, ofstream& outfile, int rows, int columns)
{
	for (int y = columns - 1; y >= 0; y--)
	{
		for (int x = 0; x < rows; x++)
		{
			int i = x + (y * rows);

			outfile << (int)data[i][0] << " " << (int)data[i][1] << " " << (int)data[i][2] << (x == 0 ? "\n" : " ");
		}
	}
}

void applyHorizontalBlur(const vector<pixel>& data, ofstream& outfile, int rows, int columns)
{

}

uint8_t computeNegativeValue(uint8_t val)
{
	return 255 - val;
}

uint8_t computeHighContrastValue(uint8_t val)
{
	return val > 127 ? 255 : 0;
}

uint8_t computeRandomNoiseValue(uint8_t val)
{
	int v = rand() % 100;
	v -= 50;
	v += val;

	v = v < 0 ? 0 : v;
	v = v > 255 ? 255 : v;

	return (uint8_t)v;
}

uint8_t computeGrayScaleValue(pixel val)
{
	return (val[0] + val[1] + val[2]) / 3;
}
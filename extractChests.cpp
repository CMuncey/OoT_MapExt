#include <iostream>
#include "endian.h"
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <cmath>
using namespace std;

int AtoH(char);
bool ErrorCheck(int, char**);

int main(int argc, char** argv)
{
	unsigned int start = 0, end = 0, size = 0, mapstart = 0, i = 0;
	unsigned char temp = 0;
	unsigned char* data;
	unsigned short item;
	int count, j;
	string name;
	ifstream input;
	ofstream output;
	ofstream textfile;
	ofstream chestfile;

	if(ErrorCheck(argc, argv))
		return(-1);

	//Get start and end offsets
	for(i = 0; i < 8; i++)
	{
		start += AtoH(argv[2][i]) * pow(16, 7-i);
		end += AtoH(argv[3][i]) * pow(16, 7-i);
	}

	//Write scene offsets to txt file
	name = argv[1], name += ".txt";
	textfile.open(name);
	textfile << argv[1] << ":    0x" << hex << setfill('0') << setw(8) << start << " - 0x" << setw(8) << end << endl;

	//Create file for chest offsets
	name = argv[1], name += "-Chests.txt";
	chestfile.open(name);

	//Get size, finish filename, make room for data
	size = end-start;
	data = (unsigned char*)malloc(size);
	name = argv[1], name += ".zscene";
	i = 0;
	
	//Read then write data to new file
	input.open("ZOOTDEC.z64", ios::binary);
	input.seekg(start, ios::beg);
	input.read(reinterpret_cast<char*>(data), size);
	output.open(name, ios::binary);
	output.write(reinterpret_cast<const char*>(data), size);
	cout << name << " written" << endl;
	output.close();
	free(data);

	//Get the map starting point offset
	//If a map is inside, start halfway through line 1, if outside, start on line 2
	if(tolower(argv[4][0]) == 'o')
		input.seekg(start+16, ios::beg);
	else
		input.seekg(start+8, ios::beg);

	//The starting point for map offsets is located 4 bytes after a 04xx with xx being the number of maps for the scene
	while(temp != 0x04)
	{
		input.read(reinterpret_cast<char*>(&temp), 1);
		if(temp != 0x04)
			input.seekg(1, ios::cur);
	}
	input.read(reinterpret_cast<char*>(&temp), 1);
	input.seekg(4, ios::cur);
	input.read(reinterpret_cast<char*>(&mapstart), 2);
	count = (int)temp;
	mapstart = htobe16(mapstart);
	mapstart += start;

	//Get map offsets, make files for each
	while(i < count)
	{
		input.seekg(mapstart + (i*8), ios::beg);
		start = end = temp = j = 0;

		input.read(reinterpret_cast<char*>(&start), 4);
		input.read(reinterpret_cast<char*>(&end), 4);
		start = htobe32(start);
		end = htobe32(end);

		//Write current map offsets to txt file
		if(i < 10)
			textfile << argv[1] << "-" << dec << i << ":  0x" << hex << setfill('0') << setw(8) << start << " - 0x"  << setw(8) << end << endl;
		else
			textfile << argv[1] << "-" << dec << i << ": 0x" << hex << setfill('0') << setw(8) << start << " - 0x"  << setw(8) << end << endl;

		//Read the current map
		size = end-start;
		data = (unsigned char*)malloc(size);
		input.seekg(start, ios::beg);
		input.read(reinterpret_cast<char*>(data), size);
		name = argv[1], name += '-', name += to_string(i);

		//Find chests on map
		chestfile << name << ":" << hex << endl;
		input.seekg(start + 0x30, ios::beg);
		input.read(reinterpret_cast<char*>(&temp), 1);
		while(temp != 0x14)
		{
			if(temp == 0x01)
			{
				input.read(reinterpret_cast<char*>(&temp), 1);
				input.seekg(4, ios::cur);
				input.read(reinterpret_cast<char*>(&end), 2);
				end = htobe16(end);
				input.seekg(start + end, ios::beg);

				while(j < (int)temp)
				{
					input.read(reinterpret_cast<char*>(&item), 2);
					item = htobe16(item);
					if(item == 0x000a)
					{
						input.seekg(12, ios::cur);
						input.read(reinterpret_cast<char*>(&item), 2);
						item = htobe16(item);
						chestfile << "   Chest 0x" << setfill('0') << setw(4) << item << " at 0x" << setw(8) << start + end + (j*16) << endl;
					}
					else
					{
						input.seekg(14, ios::cur);
					}

					j++;
				}

				break;
			}
			input.seekg(1, ios::cur);
			input.read(reinterpret_cast<char*>(&temp), 1);
		}
		if(i != count-1)
			chestfile << endl;

		name += ".zmap";
		output.open(name, ios::binary);
		output.write(reinterpret_cast<const char*>(data), size);
		cout << name << " written" << endl;
		output.close();
		free(data);
		i++;
	}

	input.close();
	name = argv[1], name += ".txt";
	cout << name << " written" << endl;
	textfile.close();
	name = argv[1], name += "-Chests.txt";
	cout << name << " written" << endl;
	chestfile.close();

	return(0);
}

int AtoH(char a)
{
	switch(tolower(a))
	{
		case '0': return(0);
		case '1': return(1);
		case '2': return(2);
		case '3': return(3);
		case '4': return(4);
		case '5': return(5);
		case '6': return(6);
		case '7': return(7);
		case '8': return(8);
		case '9': return(9);
		case 'a': return(10);
		case 'b': return(11);
		case 'c': return(12);
		case 'd': return(13);
		case 'e': return(14);
		case 'f': return(15);
	}
}

bool ErrorCheck(int argc, char** argv)
{
	ifstream input;

	if(argc != 5)
	{
		printf("Usage: (Name of map) (Start offset) (End offset) (Indoor/Outdoor)\n");
		return(true);
	}

	if(tolower(argv[4][0]) != 'i' and tolower(argv[4][0]) != 'o')
	{
		printf("Error: Need to specify indoor or outdoor map\n");
		return(true);
	}

	if(strlen(argv[2]) != 8)
	{
		printf("Error: Starting offset needs to be 4 bytes (8 characters)\n");
		return(true);
	}

	if(strlen(argv[3]) != 8)
	{
		printf("Error: Ending offset needs to be 4 bytes (8 characters)\n");
		return(true);
	}

	input.open("ZOOTDEC.z64", ios::binary);
	if(input.fail())
	{
		perror("ZOOTDEC.z64");
		input.close();
		return(true);
	}
	input.close();

	return(false);
}

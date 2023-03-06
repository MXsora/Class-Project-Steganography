#include "pch.h"
#include <iostream>
#include <omp.h>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <chrono>
#include <stdio.h>
using namespace std;

int threadNum = 1;

//prompts the user with a choice that'll dictate whether we encode or decode
int userChoice() {
	int choice = 0;
	cout << "Hello! How many threads do want? 1, 2, 4, 8, 16?" << endl;
	cin >> choice;
	cin.ignore();
	if (choice != 1 && choice != 2 && choice != 4 && choice != 8 && choice != 16) {
		cout << "Choice must be one of the numbers presented: 1, 2, 4, 8, 16" << endl;
		return userChoice();
	}
	return choice;
}

//gets the message from the console and returns that massage as a string
string getMessage() {
	auto message = string{};
	getline(cin, message);
	return message;
}

//gets the file path from the console and returns that path as a string
//note that fowards slashes should be used to seperate folders instead of back slashes
string getFilePath() {
	auto filePath = string{};
	getline(cin, filePath);
	return filePath;
}

//reads a given file and returns a vector of its data
vector<unsigned __int8> read(string fileName) {
	auto vec = vector<unsigned __int8>{};
	auto file = ifstream(fileName);
	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	auto buffer = new char[size];
	file.read(buffer, size);
	vec.insert(vec.end(), buffer, buffer + size);
	delete[] buffer;
	return vec;
}

string readLargeTextFile(string filePath) {
	auto txtFile = ifstream(filePath);
	if (!txtFile.is_open()) {
		cout << "Couldn't find file. Exiting out." << endl;
		return "failed";
	}
	txtFile.seekg(0, txtFile.end);
	int size = txtFile.tellg();
	txtFile.seekg(0, txtFile.beg);
	auto buffer = new char[size];
	txtFile.read(buffer, size);
	auto longMessage = string(buffer);
	delete[] buffer;
	return longMessage;
}

//this reads a vector of data from a starting position and consolidates the characters it finds until it hits any empty space delimiter
string readUntilWhiteSpace(vector<unsigned __int8> image, int start) {
	auto output = string{};
	int i = start;
	while (true) {
		auto character = image[i];
		if (character == 9 || character == 10 || character == 13 || character == 32) {
			break;
		}
		output += character;
		i++;
	}
	return output;
}

//encodes a given message into the given vector of the image and returns a new vector
vector<unsigned __int8> encodeImage(vector<unsigned __int8> image, vector<unsigned __int8> message) {
	int start = 0;
	auto encoded = vector<unsigned __int8>(image.size(), 0);

	auto magicNumber = readUntilWhiteSpace(image, start);
	start += magicNumber.length() + 1;
	auto widthString = readUntilWhiteSpace(image, start);
	start += widthString.length() + 1;
	auto heightString = readUntilWhiteSpace(image, start);
	start += heightString.length() + 1;
	auto maxColorString = readUntilWhiteSpace(image, start);
	start += maxColorString.length() + 1;

	for (int i = 0; i < start; i++) {
		encoded[i] = image[i];
	}
	auto messageSize = message.size();
	auto end = image.size() - start;
	if (end < messageSize) {
		cout << "The message is too big for this size of image. This operation will continue but try again and either provide a bigger image or shorten the message." << endl;
	}
#pragma omp parallel num_threads(threadNum)
	{
#pragma omp for
		for (int i = 0; i < end; i++) {
			auto color = image[i + start];
			color &= 254;
			if (i < messageSize) {
				color |= message[i];
			}
			encoded[i + start] = color;
		}
	}
	return encoded;
}

//grabs the least significant bit of each color channel from the given image and returns a new vector of the consolidated bits
vector<unsigned __int8> getMessageFromEncodedImage(vector<unsigned __int8> image) {
	int start = 0;
	auto message = vector<unsigned __int8>(image.size(), 0);

	auto magicNumber = readUntilWhiteSpace(image, start);
	start += magicNumber.length() + 1;
	auto widthString = readUntilWhiteSpace(image, start);
	start += widthString.length() + 1;
	auto heightString = readUntilWhiteSpace(image, start);
	start += heightString.length() + 1;
	auto maxColorString = readUntilWhiteSpace(image, start);
	start += maxColorString.length() + 1;

	auto end = image.size() - start;
#pragma omp parallel num_threads(threadNum)
	{
		int x = 0;
#pragma omp for
		for (int i = 0; i < end; i++) {
			auto color = image[i + start];
			color &= 1;
			if (color == 0) {
				x++;
				if (x > 7) {
					break;
				}
			}
			message[i] = color;
		}
	}
	return message;
}

//writes a new image with an array of ints
void writeNewImage(vector<unsigned __int8> imageArr, string filePath) {
	ofstream newImage;
	newImage.open(filePath, ios::binary);
	if (!newImage) {
		cout << "Could not create new file";
		return;
	}
	cout << "File created successfuly" << endl;
	newImage.write((char*)imageArr.data(), imageArr.size());
}

//iterates over the message and places it into the messageBitArray
//it places each 8 bits in least significant bit order so 0-7, 8-15, etc need to be read high to low in order to get the correct byte
vector<unsigned __int8> convertMessageToBits(string message) {
	auto messageBitArray = vector<unsigned __int8>(message.length() * 8, 0);
#pragma omp parallel num_threads(threadNum)
	{
#pragma omp for
		for (int i = 0; i < message.size(); i++)
		{
			auto bitMessage = bitset<8>(message.c_str()[i]);
			for (int j = 0; j < 8; j++) {
				messageBitArray[j + (i * 8)] = bitMessage[j];
			}
		}
	}
	return messageBitArray;
}

//decodes an array of bits into the original message assuming the data is given from least to most significant bit
string decodeArray(vector<unsigned __int8> &messageBitArray) {
	auto decodedMessage = string(messageBitArray.size(), '\0');
#pragma omp parallel num_threads(threadNum)
	{
#pragma omp for
		for (int i = 0; i < messageBitArray.size() / 8; i++)
		{
			int decimalValue = 0;
			int multiplier = 1;
			int bitChar[8];
			for (int j = 0; j < 8; j++) {
				bitChar[j] = messageBitArray[j + (i * 8)];
			}
			for (int j = 0; j < 8; j++) {
				decimalValue += bitChar[j] * multiplier;
				multiplier = multiplier * 2;
			}
			decodedMessage[i] = (char)decimalValue;
		}
	}
	auto nullIndex = decodedMessage.find('\0');
	return decodedMessage.erase(nullIndex, decodedMessage.size());
}

//prints an array of bits in sections of 8 to represent bytes
//prints the array from most to least significant bit so the array passed will have to be made least to most
//mostly just to verify the data is correct
void printArrayBytes(vector<int> &messageBitArray) {
	for (int i = 0; i < messageBitArray.size() / 8; i++) {
		for (int x = 7; x > -1; x--) {
			cout << messageBitArray[x + (i * 8)];
		}
		cout << endl;
	}
}

int main()
{
	threadNum = userChoice();
	auto longMessage = readLargeTextFile("hugeMessage.txt");
	auto start = chrono::high_resolution_clock::now();
	auto bits = convertMessageToBits(longMessage);		
	string filePath = "hugeImage.ppm";		
	auto image = read(filePath);					
	auto encoded = encodeImage(image, bits);		
	filePath = "hugeEncodedImage.ppm";
	writeNewImage(encoded, filePath);				
	cout << "Encoding phase successful, onto decoding" << endl;
	auto encodedImage = read(filePath);										
	auto messageBits = getMessageFromEncodedImage(encodedImage);						
	auto message = decodeArray(messageBits);
	auto end = chrono::high_resolution_clock::now();
	cout << "This process with " << threadNum << " threads took " << chrono::duration<double, std::milli>(end - start).count() << " milliseconds\n";
}
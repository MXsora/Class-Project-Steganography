#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <stdio.h>
using namespace std;

////prompts the user with a choice that'll dictate whether we encode or decode
//int userChoice() {
//	int choice = 0;
//	cout << "Hello! Would you like to encode a message into an image or decode an image into a message?" << endl;
//	cout << "Type 1 for encoding and 2 for decoding." << endl << "Choice: ";
//	cin >> choice;
//	cin.ignore();
//	if (choice != 1 && choice != 2) {
//		return userChoice();
//	}
//	return choice;
//}
//
////prompts the user to type out the message
//void promptUser(int x) {
//	switch (x) {
//	case 1:			//encode prompt
//		cout << "Enter your message to be encrypted below." << endl << "Message: ";
//		break;
//	case 2:			//decode prompt
//		cout << "Enter the path to the file you want to decode. Include the filename in the path." << endl << "File path: ";
//		break;
//	case 3:			//grab image to encode into prompt
//		cout << "Enter the path to the file you want to encode into. Include the filename in the path." << endl << "File path: ";
//		break;
//	case 4:			//save newly encoded image prompt
//		cout << "Enter the path to the folder where you want to save the encoded image. Give a new name for the file and include .ppm at the end" << endl << "File path: ";
//		break;
//	}
//}
//
////gets the message from the console and returns that massage as a string
//string getMessage() {
//	auto message = string{};
//	getline(cin, message);
//	return message;
//}
//
////gets the file path from the console and returns that path as a string
////note that fowards slashes should be used to seperate folders instead of back slashes
//string getFilePath() {
//	auto filePath = string{};
//	getline(cin, filePath);
//	return filePath;
//}
//
////reads a given file and returns a vector of its data
//vector<unsigned __int8> read(string fileName) {
//	auto vec = vector<unsigned __int8>{};
//	auto file = ifstream(fileName);
//	file.seekg(0, file.end);
//	int size = file.tellg();
//	file.seekg(0, file.beg);
//	auto buffer = new char[size];
//	file.read(buffer, size);
//	vec.insert(vec.end(), buffer, buffer + size);
//	delete[] buffer;
//	return vec;
//}
//
////this reads a vector of data from a starting position and consolidates the characters it finds until it hits any empty space delimiter
//string readUntilWhiteSpace(vector<unsigned __int8> image, int start) {
//	auto output = string{};
//	int i = start;
//	while (true) {
//		auto character = image[i];
//		if (character == 9 || character == 10 || character == 13 || character == 32) {
//			break;
//		}
//		output += character;
//		i++;
//	}
//	return output;
//}
//
////encodes a given message into the given vector of the image and returns a new vector
//vector<unsigned __int8> encodeImage(vector<unsigned __int8> image, vector<unsigned __int8> message) {
//	int start = 0;
//	auto encoded = vector<unsigned __int8>(image.size(), 0);
//
//	auto magicNumber = readUntilWhiteSpace(image, start);
//	start += magicNumber.length() + 1;
//	auto widthString = readUntilWhiteSpace(image, start);
//	start += widthString.length() + 1;
//	auto heightString = readUntilWhiteSpace(image, start);
//	start += heightString.length() + 1;
//	auto maxColorString = readUntilWhiteSpace(image, start);
//	start += maxColorString.length() + 1;
//
//	for (int i = 0; i < start; i++) {
//		encoded[i] = image[i];
//	}
//	auto messageSize = message.size();
//	auto end = image.size() - start;
//	if (end < messageSize) {
//		cout << "The message is too big for this size of image. This operation will continue but try again and either provide a bigger image or shorten the message." << endl;
//	}
//	for (int i = 0; i < end; i++) {
//		auto color = image[i + start];
//		color &= 254;
//		if (i < messageSize) {
//			color |= message[i];
//		}
//		encoded[i + start] = color;
//	}
//	return encoded;
//}
//
////grabs the least significant bit of each color channel from the given image and returns a new vector of the consolidated bits
//vector<unsigned __int8> getMessageFromEncodedImage(vector<unsigned __int8> image) {
//	int start = 0;
//	auto message = vector<unsigned __int8>(image.size(), 0);
//
//	auto magicNumber = readUntilWhiteSpace(image, start);
//	start += magicNumber.length() + 1;
//	auto widthString = readUntilWhiteSpace(image, start);
//	start += widthString.length() + 1;
//	auto heightString = readUntilWhiteSpace(image, start);
//	start += heightString.length() + 1;
//	auto maxColorString = readUntilWhiteSpace(image, start);
//	start += maxColorString.length() + 1;
//
//	auto end = image.size() - start;
//	int x = 0;
//	for (int i = 0; i < end; i++) {
//		auto color = image[i + start];
//		color &= 1;
//		if (color == 0) {
//			x++;
//			if (x > 7) {
//				return message;
//			}
//		}
//		message[i] = color;
//		x = 0;
//	}
//	return message;
//}
//
////writes a new image with an array of ints
//void writeNewImage(vector<unsigned __int8> imageArr, string filePath) {
//	ofstream newImage;
//	newImage.open(filePath, ios::binary);
//	if (!newImage) {
//		cout << "Could not create new file" << endl;
//		return;
//	}
//	cout << "File created successfuly" << endl;
//	newImage.write((char*)imageArr.data(), imageArr.size());
//}
//
////iterates over the message and places it into the messageBitArray
////it places each 8 bits in least significant bit order so 0-7, 8-15, etc need to be read high to low in order to get the correct byte
//vector<unsigned __int8> convertMessageToBits(string message) {
//	auto messageBitArray = vector<unsigned __int8>(message.length() * 8, 0);
//	for (int i = 0; i < message.size(); i++)					
//	{
//		auto bitMessage = bitset<8>(message.c_str()[i]);
//		for (int j = 0; j < 8; j++) {
//			messageBitArray[j + (i * 8)] = bitMessage[j];
//		}
//	}
//	return messageBitArray;
//}
//
////decodes an array of bits into the original message assuming the data is given from least to most significant bit
//string decodeArray(vector<unsigned __int8> &messageBitArray) {
//	auto decodedMessage = string{};
//	for (int i = 0; i < messageBitArray.size() / 8; i++)
//	{
//		int decimalValue = 0;
//		int multiplier = 1;
//		int bitChar[8];
//		for (int j = 0; j < 8; j++) {
//			bitChar[j] = messageBitArray[j + (i * 8)];
//		}
//		for (int j = 0; j < 8; j++) {
//			decimalValue += bitChar[j] * multiplier;
//			multiplier = multiplier * 2;
//		}
//		decodedMessage += (char)decimalValue;
//	}
//	return decodedMessage;
//}
//
////prints an array of bits in sections of 8 to represent bytes
////prints the array from most to least significant bit so the array passed will have to be made least to most
////mostly just to verify the data is correct
//void printArrayBytes(vector<int> &messageBitArray) {
//	for (int i = 0; i < messageBitArray.size() / 8; i++) {
//		for (int x = 7; x > -1; x--) {
//			cout << messageBitArray[x + (i * 8)];
//		}
//		cout << endl;
//	}
//}
//
//int main()
//{
//	if (userChoice() == 1) {							//prompt the user for whether we encode or decode
//		promptUser(1);									//prompt the user to type in the message
//		auto message = getMessage();					//grab the message
//		auto bits = convertMessageToBits(message);		//convert that message to a vector representing bits
//		promptUser(3);									//prompt the user to type the path for the image they want to encode into
//		auto filePath = getFilePath();					
//		auto image = read(filePath);					//read the image's data and transfer it into a vactor
//		auto encoded = encodeImage(image, bits);		//take that vector and encode the message into it
//		promptUser(4);									//prompt the user to type the path of where they want to save the new image
//		filePath = getFilePath();						//get a file path for where to save the new image
//		writeNewImage(encoded, filePath);				//write a new image to that path
//	}
//	else {
//		promptUser(2);														//prompt user for the file location of the image to decode
//		auto filePath = getFilePath();										
//		auto image = read(filePath);										//convert that image to a vector
//		auto bits = getMessageFromEncodedImage(image);						//get the message in bits from the image
//		auto message = decodeArray(bits);									//decode that message from bits
//		cout << endl << "The encoded message is:   " << message << endl;
//	}
//}
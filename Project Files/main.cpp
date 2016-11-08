#include <iostream>

//! Include the Blackboard
#include "Blackboard.h"
using Utilities::Blackboard;

//! Define a simple function to print out a number
inline void printInt(const int& pNum) { std::cout << "Print Int: " << pNum << std::endl; }

int main() {
	//Seed the random number generator
	srand((unsigned int)time(NULL));

	//Create the black board
	if (Blackboard::create()) {
		//Write a random number
		Blackboard::write("Number", rand());

		//Read out the number
		std::cout << "Number: " << Blackboard::read<int>("Number") << std::endl;

		//Read out unused key as float
		std::cout << "Unused: " << Blackboard::read<float>("Other") << std::endl;

		//Add the callback function for number
		Blackboard::subscribe<int>("Number", printInt);

		//Modify the number at 'Number'
		Blackboard::write("Number", rand());
	}

	//Destroy the blackboard
	Blackboard::destroy();

	//Pause and wait for exit
	system("PAUSE");
	return EXIT_SUCCESS;
}

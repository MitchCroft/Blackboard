#include <iostream>

//! Include the Blackboard
#include "Blackboard.h"
using Utilities::Blackboard;

#pragma region 0. Creation
/*
	exercise0 - Test the creation and destruction of the Blackboard
	Author: Mitchell Croft
	Created: 08/11/2016
	Modified: 08/11/2016
*/
void exercise0() {
	//Create the Blackboard
	std::cout << (Blackboard::create() ? 
										"The Blackboard was created successfully..." :
										"The Blackboard failed to create successfully...") << 
																							  std::endl;

	//Output the destruction message
	std::cout << "Destroying the Blackboard" << std::endl;

	//Destroy the Blackboard
	Blackboard::destroy();
}
#pragma endregion

#pragma region 1. Read/Write
/*
	exercise1 - Test the writing to and reading of values to the Blackboard
	Author: Mitchell Croft
	Created: 08/11/2016
	Modified: 08/11/2016
*/
void exercise1() {
	//Define basic color struct
	struct Color { union { struct { unsigned char r, g, b, a; }; unsigned int colorID; }; };

	//Create the Blackboard
	if (Blackboard::create()) {
		//Output success message
		std::cout << "Successfully created the Blackboard..." << std::endl;

		int usrInt;
		float usrFlt;
		char* usrStr = new char[64];
		Color usrStruct;

		//Prompt the user for an integer
		std::cout << "Please enter an integer value: ";
		std::cin >> usrInt;

		//Write the integer to the Blackboard
		Blackboard::write("UserInteger", usrInt);

		//Prompt the user for a float
		std::cout << "Please enter a float value: ";
		std::cin >> usrFlt;

		//Write the float to the Blackboard
		Blackboard::write("UserFloat", usrFlt);

		//Prompt the user for a string
		std::cout << "Please enter a word (Maximum characters 63): ";
		std::cin.get(usrStr, sizeof(char) * 64);

		//Write the string address to the Blackboard
		Blackboard::write("UserValue", usrStr);

		//Prompt the user for a Color ID
		std::cout << "Please enter a Color ID (32bit integer containing R, G, B, A values): ";
		std::cin >> usrStruct.colorID;

		//Write the color struct to the Blackboard
		Blackboard::write("UserValue", usrStruct);

		//Output the collected information 
		std::cout << "The recorded integer value was " << Blackboard::read<int>("UserInteger") << std::endl;
		std::cout << "The recorded float value was " << Blackboard::read<float>("UserFloat") << std::endl;
		std::cout << "The recorded string value was " << Blackboard::read<char*>("UserValue") << std::endl;
		
		//Save the read out struct values
		Color read = Blackboard::read<Color>("UserValue");

		//Output the color information
		std::cout << "The recorded Color ID was " << read.colorID << " which results in an RGBA set of (" << (int)read.r << ", " << (int)read.g << ", " << (int)read.b << ", " << (int)read.a << ")" << std::endl;

		//Clear memory
		delete[] usrStr;
	}

	//Output failed message
	else std::cout << "Failed to create the Blackboard...." << std::endl;

	//Output destruction message
	std::cout << "Destroying the Blackboard..." << std::endl;

	//Destroy the Blackboard
	Blackboard::destroy();
}
#pragma endregion

/*
 *		Name: UserTest 
 *		Author: Mitchell Croft
 *		Created: 08/11/2016
 *		Modified: 08/11/2016
 *		
 *		Purpose:
 *		Store basic information to display to the user in order
 *		to run a test
**/
struct UserTest { std::string name; void(*exerciseFunc)(); };

/*
	main - Start and manage the application so the functionality of the Blackboard
		   can be tested
	Author: Mitchell Croft
	Created: 08/11/2016
	Modified: 08/11/2016

	return int - Returns the success state of the program
*/
int main() {
	//Seed the random number generator
	srand((unsigned int)time(NULL));

	//Create an array of the possible tests to run
	UserTest tests[] = {
		{"Creation", exercise0},
		{"Read/Write", exercise1},
	};

	//Store the total number of tests in the array
	const short TEST_COUNT = (sizeof(tests) / sizeof(*tests));

	//Create a variable to store the users choice
	short usrChoice;

	//Loop until the user exits
	do {
		//Clear the screen of all visible data
		system("CLS");

		//Print out the tests that can be run
		std::cout << "Available Tests (Total " << TEST_COUNT << "):" << std::endl;
		for (short i = 0; i < TEST_COUNT; i++)
			std::cout << i << ". " << tests[i].name.c_str() << std::endl;
		std::cout << std::endl << std::endl;

		//Prompt the user for a response
		std::cout << "Enter a valid number for the test to run: ";

		//Take in user response
		std::cin >> usrChoice;

		//Check the bounds
		if (usrChoice >= 0 && usrChoice < TEST_COUNT) {
			//Add a bunch of line breaks
			std::cout << std::endl << std::endl << std::endl << std::endl << std::endl;

			//Run the exercise function
			tests[usrChoice].exerciseFunc();

			//Pause the system and wait for the user to respond
			system("PAUSE");
		}

	} while (usrChoice >= 0);

	//Exit successful
	return EXIT_SUCCESS;
}

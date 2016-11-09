#include <iostream>

//! Include the Blackboard
#include "Blackboard.h"
using Utilities::Blackboard;

#pragma region Input Functionality
/*
    clearInBuffer - Clear the buffer of characters taken in via std::cin
    Author: Mitchell Croft
    Created: 09/11/2016
    Modified: 09/11/2016
*/
inline void clearInBuffer() {
    //Reset stream state
    std::cin.clear();

    //Ignore to the EOF
    std::cin.ignore(INT_MAX, '\n');
}

/*
    getInput - Receives input from the user of an indiscriminate type
    Author: Mitchell Croft
    Created: 09/11/2016
    Modified: 09/11/2016

    template T - A non char* type

    param[in] pIn - A reference to the variable to store the users input in
    param[in] pMessage - An optional char pointer of a message to display 
                         prior to prompting the user for input (Default nullptr)
    param[in] pBufferSize - Unused parameter to store the size of an array
*/
template<typename T>
void getInput(T& pIn, const char* pMessage = nullptr, size_t pBufferSize = INT_MAX) {
    //If there is an output message print it out
    if (pMessage) std::cout << pMessage;

    //Read in the user response
    std::cin >> pIn;

    //Reset the stream state
    clearInBuffer();
}

/*
    getInput - Receives string input from the user
    Author: Mitchell Croft
    Created: 09/11/2016
    Modified: 09/11/2016

    template char* - Specilisation for char* 

    param[in] pIn - A reference to the char pointer to store the users input
    param[in] pMessage - An optional char pointer of a message to display
                         prior to prompting the user for input (Default nullptr)
    param[in] pBufferSize - The size of the char buffer that is being filled
*/
template<>
void getInput(char*& pIn, const char* pMessage, size_t pBufferSize) {
    //If there is an output message, print it out
    if (pMessage) std::cout << pMessage;

    //Read in the user response
    std::cin.get(pIn, sizeof(char) * pBufferSize);

    //Reset the stream size
    clearInBuffer();
}
#pragma endregion

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
    Modified: 09/11/2016
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
        char* usrStr = new char[33];
        Color usrStruct;

        //Prompt the user for an integer
        getInput(usrInt, "Please enter an integer value: ");

        //Write the integer to the Blackboard
        Blackboard::write("UserInteger", usrInt);

        //Prompt the user for a float
        getInput(usrFlt, "Please enter a float value: ");

        //Write the float to the Blackboard
        Blackboard::write("UserFloat", usrFlt);

        //Prompt the user for a string
        getInput(usrStr, "Please enter a word (Maximum characters 32): ", 33);

        //Write the string address to the Blackboard
        Blackboard::write("UserValue", usrStr);

        //Prompt the user for a Color ID
        getInput(usrStruct.colorID, "Please enter a Color ID (32bit integer containing R, G, B, A values): ");

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

        //Clear allocated memory
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

#pragma region 2. Key Writing/Wiping
/*
    exercise2 - Test the writing to, wiping of and reading of key values
    Author: Mitchell Croft
    Created: 09/11/2016
    Modified: 09/11/2016
*/
void exercise2() {
    //Define an enum for the different options
    enum EExercise2Options { QUIT = -1, Write_Val, Read_Val, Wipe_Key, Wipe_Key_Type, Wipe_All, EX_TOTAL };

    //Define an enum for the different types
    enum ETypeOptions : unsigned short { Int, Float, Double, Short, Char, TY_TOTAL };

    //Create the Blackboard
    if (Blackboard::create()) {
        //Output success message
        std::cout << "Successfully created the Blackboard..." << std::endl;

        //Store the users input choice
        int usrChoice;

        //Loop until the user quits
        do {
            do {
                //Output some line breaks
                std::cout << std::endl << std::endl;

                //Output options for the user
                std::cout << "Choose an option (-1 to quit):" << std::endl;
                std::cout << "0. Write value" << std::endl;
                std::cout << "1. Read Value" << std::endl;
                std::cout << "2. Wipe Key" << std::endl;
                std::cout << "3. Wipe Key of Type" << std::endl;
                std::cout << "4. Wipe All Values" << std::endl << std::endl;

                //Read in the user input
                getInput(usrChoice, "What would you like to do: ");
            } while (usrChoice >= EExercise2Options::EX_TOTAL);

            //Break if user quits
            if (usrChoice <= EExercise2Options::QUIT) break;

            //If user chose a specific value to modify
            if (usrChoice != EExercise2Options::Wipe_All) {
                //Store a key to manipulate
                char modifyKey[33] = { '\0' };

                //Read in the key from the user
                do {
                    //Add a line break
                    std::cout << std::endl;

                    //Get the key from the user
                    getInput(modifyKey, "Enter the Key Value to modify (Maximum 32 characters): ", 33);
                } while (!strlen(modifyKey));

                //Add a line break
                std::cout << std::endl;

                //Determine the type specific actions
                if (usrChoice != EExercise2Options::Wipe_Key) {
                    //Store the type that the user wants to modify
                    unsigned short usrType;

                    //Read in type from the user
                    do {
                        //Add a line break
                        std::cout << std::endl;

                        //Output the options
                        std::cout << "Please select the type that you would like to use:" << std::endl;
                        std::cout << "0. Int" << std::endl;
                        std::cout << "1. Float" << std::endl;
                        std::cout << "2. Double" << std::endl;
                        std::cout << "3. Short" << std::endl;
                        std::cout << "4. Char" << std::endl << std::endl;

                        //Get the user input
                        getInput(usrType, "Please select a type to use: ");
                    } while (usrType >= ETypeOptions::TY_TOTAL);

                    //Check if the user is not writing a value
                    if (usrChoice != EExercise2Options::Write_Val) {
                       //Switch based on the remaining actions
                        switch (usrChoice) {
                        case Read_Val:
                            //Output start of the message
                            std::cout << "Reading from the Blackboard, the value stored at the key '" << modifyKey << "' with the type ";

                            //Switch based on the type selected
                            switch (usrType) {
                            case Int: std::cout << "Int is " << Blackboard::read<int>(modifyKey) << std::endl; break;
                            case Float: std::cout << "Float is " << Blackboard::read<float>(modifyKey) << std::endl; break;
                            case Double: std::cout << "Double is " << Blackboard::read<double>(modifyKey) << std::endl; break;
                            case Short: std::cout << "Short is " << Blackboard::read<short>(modifyKey) << std::endl; break;
                            case Char: std::cout << "Char is " << Blackboard::read<char>(modifyKey) << std::endl; break;
                            }
                            
                            break;
                        case Wipe_Key_Type:
                            //Output the start of the message
                            std::cout << "Wiping the key '" << modifyKey << "' from the Blackboard for the type ";

                            //Switch based on the type selected
                            switch (usrType) {
                            case Int: std::cout << "Int..." << std::endl; Blackboard::wipeTypeKey<int>(modifyKey); break;
                            case Float: std::cout << "Float..." << std::endl; Blackboard::wipeTypeKey<float>(modifyKey); break;
                            case Double: std::cout << "Double..." << std::endl; Blackboard::wipeTypeKey<double>(modifyKey); break;
                            case Short: std::cout << "Short..." << std::endl; Blackboard::wipeTypeKey<short>(modifyKey); break;
                            case Char: std::cout << "Char..." << std::endl; Blackboard::wipeTypeKey<char>(modifyKey); break;
                            }
                            break;
                        }
                    }

                    //Otherwise find a value to write
                    else {
                        //Create containers for the different types
                        int usrInt;
                        float usrFloat;
                        double usrDbl;
                        short usrShrt;
                        char usrChar;

                        //Add a line break
                        std::cout << std::endl;

                        //Switch based on the type to input
                        switch (usrType) {
                        case Int: getInput(usrInt, "Please enter the int value to write: ");    break;
                        case Float: getInput(usrFloat, "Please enter the float value to write: "); break;
                        case Double: getInput(usrDbl, "Please enter the double value to write: "); break;
                        case Short: getInput(usrShrt, "Please enter the short value to write: "); break;
                        case Char: getInput(usrChar, "Please enter the char value to write: "); break;
                        }

                        //Output writing message
                        std::cout << "Writing the value to the Blackboard..." << std::endl;

                        //Switch on the type being written
                        switch (usrType) {
                        case Int: Blackboard::write(modifyKey, usrInt, false); break;
                        case Float: Blackboard::write(modifyKey, usrFloat, false); break;
                        case Double: Blackboard::write(modifyKey, usrDbl, false); break;
                        case Short: Blackboard::write(modifyKey, usrShrt, false); break;
                        case Char: Blackboard::write(modifyKey, usrChar, false); break;
                        }
                    }
                }

                //Otherwise erase all values with the specified key
                else {
                    //Output erasing message
                    std::cout << "Erasing all values with the key '" << modifyKey << "'" << std::endl;

                    //Erase all values with the specified key
                    Blackboard::wipeKey(modifyKey);
                }
            }

            //Otherwise clear all values on the Blackboard
            else {
                //Output erasing message
                std::cout << "Erasing all values stored on the Blackboard" << std::endl;

                //Erase all values
                Blackboard::wipeBoard();
            }

        } while (usrChoice >= 0);
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
 *      Name: UserTest 
 *      Author: Mitchell Croft
 *      Created: 08/11/2016
 *      Modified: 08/11/2016
 *      
 *      Purpose:
 *      Store basic information to display to the user in order
 *      to run a test
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
        {"Key Writing/Wiping", exercise2},
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

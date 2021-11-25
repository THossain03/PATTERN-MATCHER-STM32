// Code for ECE 198 Project- PATTERN MATCHER

// Written by Tameem Hossain and Tahmid Ahmed, November 2021
// with sample code help from Bernie Roehl, August 2021

// To run a particular example, you should remove the comment (//) in
// front of exactly ONE of the following lines below.
// Only the examples that the game used concepts for (from the original sample code) remain and are commented out for reference.
// the defined PATTERN_MATCH is however our own game code made by Tameem and Tahmid.  

#define PATTERN_MATCH   // Main game code that uses loops to show success at each level, and whether they have successfully unlocked the lock or not.
//#define BUTTON_BLINK    --> concepts were used
//#define LIGHT_SCHEDULER   --> concepts were used
//#define TIME_RAND    --> concepts were used
// #define KEYPAD   --> concepts were used
//#define KEYPAD_CONTROL    --> concepts were used

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"


//The following are functions we have created in order to help us create the complexities of each level of the game, and actually run each level itself.
//Input detection and output generation was all done in these following functions, while the iteration of levels was mainly done in the defined PATTERN_MATCH.

int random_int(int min, int max, long randnum);
int random_int(int min, int max, long randnum) { //a random number generator between a max and min value. (of int type)
   return min+((randnum)%(max-min+1));
}

int sequence_lengthGENERATOR(int lvl_num);
int sequence_lengthGENERATOR(int lvl_num) { //generates a sequence will be between 3-4, 5-6, 7-8, 9-10, 11-12 depending on the level
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)); //awaits the blue button press to develop a length, note here this is only called before outputting begins.
    srand(HAL_GetTick());
    if (lvl_num==1) { //Level 1 the sequencing willing be between 3-4
        return random_int(3,4,random());
    }else if (lvl_num==2){ //Level 2 the sequencing willing be between 5-6
        return random_int(5,6,random());
    }else if (lvl_num==3){ //Level 3 the sequencing willing be between 7-8
        return random_int(7,8,random());
    }else if (lvl_num==4){ //Level 4 the sequencing willing be between 9-10
        return random_int(9,10,random());
    }else{ //Level 5 the sequencing willing be between 11-12
        return random_int(11,12,random());
    }
}

int * rand_output_generation(int size);
int * rand_output_generation(int size) { //assigns the sequence of indexes of ports at which light will be flashed
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  //awaits blue button press as mentioned in the serial port.
    srand(HAL_GetTick());
    int * array;
    array = malloc(size); //allocating the new array pointer.
    for (int i=0; i<size; i++) {  //assign random corresponding light number to blink.
        HAL_Delay(random_int(5,9, random()));
        array[i] = random_int(1,6, random());
    }
    return array;
    free(array); //de-allocating the array.
}

//push back removal
void output_by_LED(int LED_indx, int lvl_num);  //outputting every specified LED for varied seconds as levels progress. 
void output_by_LED(int LED_indx, int lvl_num) {
    HAL_Delay(400); // an initial delay of 400ms to allow users have time to settle prior to level.
    //initializing each pin to the LED number indicated in the breadboards.
    InitializePin(GPIOA, GPIO_PIN_1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //LED-1
    InitializePin(GPIOA, GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //LED-2
    InitializePin(GPIOA, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //LED-3
    InitializePin(GPIOB, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //LED-4
    InitializePin(GPIOA, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //LED-5
    InitializePin(GPIOA, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); //LED-6
    int sec;
    //The following if-statments will reduce the time the led flashes for as the levels progress 
    //(our program will make flashing time decrease by 250 ms each time)
    if(lvl_num==1){
        sec=1250;
    }else if (lvl_num==2){
        sec=1000;
    }else if (lvl_num==3){
        sec=750;
    }else if (lvl_num==4){
        sec=500;
    }else{
        sec=250;
    }
    
    if (LED_indx==1) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, true);
        HAL_Delay(sec);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, false);
    } else if (LED_indx==2) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, true);
        HAL_Delay(sec);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, false);
    } else if (LED_indx==3) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, true);
        HAL_Delay(sec);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, false);
    } else if (LED_indx==4) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, true);
        HAL_Delay(sec);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, false);
    } else if (LED_indx==5) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, true);
        HAL_Delay(sec);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, false);
    } else if (LED_indx==6) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, true);
        HAL_Delay(sec);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, false);
    } else {  //we do not want a frustration for users in instances where they see they did it right, but got it wrong according to the program.
        SerialPuts("\n***\nAn error has occurred. Lock will be terminated immediately. Please try again.");
        exit(0); 
    }
}

bool compare(int outputs [], int inputs [], int currIndx);
bool compare(int outputs [], int inputs [], int currIndx) { //compares input array to output array after the end of input event handling. 
    if (outputs[currIndx]==inputs[currIndx]) {
        return true;
    } else {
        return false;
    }
}

bool level(int lvl_num) {  //main code for one level iteration
    if(lvl_num == 1) {//general Description of lock given to users via Serial Port (only if at level 1)
        SerialPuts("\nWelcome to the Pattern Matcher lock. For this stage in the escape room game, \n");
        SerialPuts("you will have to correctly detect the Pattern of lights being presented.\n");
        SerialPuts("Use the keypad at hand to input your anwers. Each LED has been assigned a number 1-6.\n");
        SerialPuts("To exit the game, Press A on the Keypad.\n");
        SerialPuts("You will need to pass all three levels in order to break through the lock. Good luck!\n\n");
    }
    SerialPuts("(Press blue button on board to start level ");
    SerialPutc(lvl_num+48);
    SerialPuts(")\n\n"); 

    size_t num_elements = sequence_lengthGENERATOR(lvl_num);  // generate a length that will be used by this variable throughout the level.

    //output the lights (using pins and ports)
    int * outputIndx_Arr; //array of output LEDS to be flashed. (1-6 only)
    outputIndx_Arr = malloc(num_elements); //allocation of the output array
    outputIndx_Arr = rand_output_generation(num_elements); 
    for(int i=0; i<num_elements; i++) {
        output_by_LED(outputIndx_Arr[i], lvl_num);
        //SerialPutc(outputIndx_Arr[i]+48);  //***** REMOVE THE COMMENT IF THE OUTPUT ARRAY WANTS TO BE SEEN BY USER (MAINLY FOR TESTING)
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);
    HAL_Delay(300);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5,false); //to dedicate a message through hardware that outputting is finished.

    //input array declarations.
    int elements[num_elements];//main array for keypad input elements.
    // NOTE: int elements is not implemented as a dynamically allocated pointer array as it lead us to experiencing many errors with the other
    // allocated outputIndx_Arr aray. This may lead to possible memory leaks (only for the input systems), although highly unlikely given 
    // we still are able to at least de-allocate the data after each iteration of level().

    
    HAL_Delay(100); //Waits 1/10 of a second before initializing the keypad.

    //INPUT generation begins.
    InitializeKeypad(); // initializes the keypad for inputs
    while (true)
    {
        char *keypad_symbols = "123A456B789C*0#D"; //used from KEYPAD() function.
        for (int count=0; count<num_elements; count++) {
            elements[count] = 999; //initializes each elements value originally as 999.
        }
        for (int i=0; i<num_elements; i++) {
            while (ReadKeypad() < 0);   // wait for a valid key.
            int key = ReadKeypad();
            SerialPutc(keypad_symbols[key]);  // look up its ASCII symbol and send it to the host.
            if (key == 3) { // if A is pressed, exit game
                int r=0;
                SerialPuts("\n\nEnding game. Hope to see you try again.");
                while (r<6) // blinking the green on-board LED 3 times to indicate exit.
                {
                    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); 
                    HAL_Delay(500);  // 250 milliseconds == 1/4 second
                    r++;
                    if(r==5 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0) {
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);  //makes sure nothing else faultly flashes.
                    }
                }
                exit(0); //terminate game.
            }
            if ((key!=0) && (key!=1) && (key!=2) && (key!=4) && (key!=5) && (key!=6)) {  //1-6 is only valid for this pattern matcher.
                SerialPuts("\nError. Input was out of range. Please enter a number between 1 and 6 only.");
                i--; //decrement i by 1 to avoid array index errors.
            }
            if (key>=0 && key<3) {  // assign input key value into index of array.
                elements[i] = (key+1);    //for 1-3
            } else if (key>=4 && key<7) {
                elements[i] = key;   //for 4-6
            }
            while (ReadKeypad() >= 0);  // wait until key is released   
        }
        for (int j=0; j<num_elements; j++) {  //check if each output and input matches.
            if (compare(outputIndx_Arr, elements, j) == false) { //if one is caught false, then output false for the level.
                SerialPuts("\nOh no! You were unable to break the lock.");
                SerialPuts("The correct combination was: "); //added here so output array can still be accessed.
                for (int k=0; k<num_elements; k++) {
                    SerialPutc(outputIndx_Arr[k]+48);
                    if (k != (num_elements-1)) {
                        SerialPuts(", ");
                    }
                }
                return false;   
            }
        }
        SerialPuts("\nGreat Work! Level passed! You have correctly determined the combination: ");
        for (int k=0; k<num_elements; k++) {
            SerialPutc(outputIndx_Arr[k]+48);
            if (k != (num_elements-1)) {
                SerialPuts(", ");
            }
        }
        return true; //else, simply return true.
    }
    //deallocations of both input and output data arrays.
    free(outputIndx_Arr);
    free(elements);
}

int main(void)
{
    int iteration_num = 1; //check which level the game is on.
    bool success; // checks success on a level/game.

    HAL_Init(); // initialize the Hardware Abstraction Layer

    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED

    // note: the on-board pushbutton is fine with the default values (no internal pull-up resistor
    // is required, since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the terminal (i.e. serial monitor) in VSCode)

    SerialSetup(9600);

    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)

#ifdef PATTERN_MATCH    
    success = level(iteration_num); //1st level call.
    while (success && iteration_num<5) { // run this for first four levels to check success.
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
        SerialPuts("\nPreparing next level...\n"); //message to serial.
        HAL_Delay(3000); //show for 3 seconds.
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);   // turn off LED
        iteration_num++;
        success = level(iteration_num);  //run the next level and find if success or not.
    } // after fifth level occurs, while loop doesn't run. But if was success, ends game and unlocks the "lock".
    if (success) {
        SerialPuts("\nCongratulations. You passed all three levels and fully unlocked the lock! Get out before the lock locks you up again!");
        //The congratulations statement above can be changed depending on purpose this pattern matcher lock is used inside the escape room.
        uint32_t now = HAL_GetTick();
        while ((HAL_GetTick()-now) < 900000) { //continue flashing the blinking LED of success for max. 15 min., users have the ability to disconnect the system or press reset button to their own wish at this point. 
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
        exit(0); //exit program automatically if nothing occurs within the 15 min.
    } else {
        SerialPuts("\nHope to see you try again!"); // they better! :)
        int r=0;
        while (r<50){ // blinking the LED 50 times to indicate exit.
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            HAL_Delay(100);  // 100 milliseconds == 1/10 second
            r++;
            if(r==5 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);  //makes sure nothing else faultly flashes.
            }
        }
        exit(0); //terminate game.
    }
#endif






// THE REST OF THE CODE FROM HERE WERE JUST USED FOR DEVELOPING CONCEPTS FOR THE GAME.
// IT HELPED FOR DEVELOPING KNOWLEDGE ON THE HARDWARE IN RESPECT TO WHAT WE TYPED IN FOR THE SOFTWARE.
//Note some of the codes from the original starter code has been changed over the course of the development of the project.

#ifdef BUTTON_BLINK
    // Wait for the user to push the blue button, then blink the LED.

    InitializePin(GPIOA, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED
    InitializePin(GPIOA, GPIO_PIN_6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);
    // wait for button press (active low)
    /*for (int i=0; i<6; i++) {
        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
        {
        }
        if (i==0) {
            HAL_GPIO_WritePin(GPIOA, GPIOP)
        }
    }*/
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
    {

    }

    while (1) // loop forever, blinking the LED
    {
        
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7); 
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);  
        HAL_Delay(1000);
    }
#endif

#ifdef LIGHT_SCHEDULER
    // Turn on the LED five seconds after reset, and turn it off again five seconds later.

    while (true) {
        uint32_t now = HAL_GetTick();
        if (now > 3000 && now < 15000)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
        else
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);  // turn off LED
    }
#endif

#ifdef TIME_RAND
    // This illustrates the use of HAL_GetTick() to get the current time,
    // plus the use of random() for random number generation.
    
    // Note that you must have "#include <stdlib.h>"" at the top of your main.c
    // in order to use the srand() and random() functions.

    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press
    srand(HAL_GetTick());  // set the random seed to be the time in ms that it took to press the button
    // if the line above is commented out, your program will get the same sequence of random numbers
    // every time you run it (which may be useful in some cases)

    while (true) // loop forever
    {
        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press

        // Display the time in milliseconds along with a random number.
        // We use the sprintf() function to put the formatted output into a buffer;
        // see https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm for more
        // information about this function
        char buff[100];
        sprintf(buff, "Time: %lu ms   Random = %ld\r\n", HAL_GetTick(), random());
        // lu == "long unsigned", ld = "long decimal", where "long" is 32 bit and "decimal" implies signed
        SerialPuts(buff); // transmit the buffer to the host computer's serial monitor in VSCode/PlatformIO

        while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button to be released
    }
#endif

#ifdef KEYPAD
    // Read buttons on the keypad and display them on the console.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    while (true)
    {
        while (ReadKeypad() < 0);   // wait for a valid key
        SerialPutc(keypad_symbols[ReadKeypad()]);  // look up its ASCII symbol and send it to the hsot
        while (ReadKeypad() >= 0);  // wait until key is released
    }
#endif

#ifdef KEYPAD_CONTROL
    // Use top-right button on 4x4 keypad (typically 'A') to toggle LED.

    InitializeKeypad();
    while (true)
    {   
        while (ReadKeypad() < 0);   // wait for a valid key
        int key = ReadKeypad();
        if (key == 3) // top-right key in a 4x4 keypad, usually 'A'
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);   // toggle LED on or off     
        while (ReadKeypad() >= 0);  // wait until key is released   
    }
#endif
    return 0;
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}

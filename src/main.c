// Sample code for ECE 198

// Written by Bernie Roehl, August 2021

// This file contains code for a number of different examples.
// Each one is surrounded by an #ifdef ... #endif block inside of main().

// To run a particular example, you should remove the comment (//) in
// front of exactly ONE of the following lines:

#define PATTERN_MATCH   // our own function to be implemented for the game.
//#define BUTTON_BLINK    //--> concepts to be used
//#define LIGHT_SCHEDULER   //--> concepts to be used
//#define TIME_RAND
// #define KEYPAD   --> concepts to be used
//#define KEYPAD_CONTROL    --> concepts to be used
// #define SEVEN_SEGMENT
// #define KEYPAD_SEVEN_SEGMENT
// #define COLOR_LED
// #define ROTARY_ENCODER
// #define ANALOG
// #define PWM

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"

int random_int(int min, int max, long randnum);
int random_int(int min, int max, long randnum) { //a random number generator between a max and min value. (of int type)
   return min+((randnum)%(max-min+1));
}
int sequence_lengthGENERATOR();
int sequence_lengthGENERATOR() { //sequence will be between 8-12 in length.
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));
    srand(HAL_GetTick());
    return random_int(8,12,random());
}

int * rand_output_generation(int (*size)());
int * rand_output_generation(int (*size)()) { //assigns the sequence of indexes of ports at which light will be flashed
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));
    srand(HAL_GetTick());
    int * array;
    array = malloc(size()); //allocating the new array pointer.
    for (int i=0; i<size(); i++) {  //assign random corresponding light number to blink.
        HAL_Delay(random_int(5,9, random()));
        array[i] = random_int(1,6, random());
        SerialPutc(array[i]+48);
    }
    return array;
    free(array); //de-allocating the array.
    array = NULL;
}

bool compare(int *outputs, int *inputs, int currIndx);
bool compare(int *outputs, int *inputs, int currIndx) {
    if (outputs[currIndx]==inputs[currIndx]) {
        return true;
    } else {
        return false;
    }
}

bool level(int lvl_num) {  //main code for one level iteration
    if(lvl_num == 1) {//general Description of lock given to users via Serial Port
        SerialPuts("\nWelcome to the Pattern Matcher lock. For this stage in the escape room game, \n");
        SerialPuts("you will have to correctly detect the Pattern of lights being presented.\n");
        SerialPuts("Use the keypad at hand to input your anwers. Each LED has been assigned a number 1-6.\n");
        SerialPuts("To exit the game, Press A on the Keypad.\n");
        SerialPuts("You will need to pass all three levels in order to break through the lock. Good luck!\n\n");
    }
    SerialPuts("(Press blue button on board to start level)\n\n");

    size_t num_elements = sequence_lengthGENERATOR();  // generate a length that will be used by this variable throughout the level.

    //output the lights (using pins and ports)
    //arrange difficulty time of output using similar code to LIGHT_SCHEDULER
    int * outputIndx_Arr = rand_output_generation(num_elements);
    for(int i=0; i<length(); i++) {
        SerialPutInt(outputIndx_Arr[i]);
    }
    //function to randomly generate array of which pin to direct to. Will use 'if' statements to further initialize each 1-6 value to a specified port.  
    

    //input generation begins.
    InitializeKeypad(); // initializes the keypad for inputs
    while (true)
    {
        char *keypad_symbols = "123A456B789C*0#D"; //used from KEYPAD() function.
        int elements[num_elements]; //main array for keypad input elements.
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
                while (r<6) // blinking the LED 3 times to indicate exit.
                {
                    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
                    HAL_Delay(500);  // 250 milliseconds == 1/4 second
                    r++;
                    if(r==5 && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0) {
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);
                    }
                }
                exit(0);
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
            if (compare(outputIndx_Arr, elements,j) == false) { //if one is caught false, then output false for the level.
                return false;   
            }
        }
        return true; //else, simply return true.
    }
    free(num_elements);
    free(outputIndx_Arr);
    outputIndx_Arr = NULL;

}

int main(void)
{
    int iteration_num = 1; //check which level the game is on.

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
    bool success = level(iteration_num);
    while (success && iteration_num<3) { // run this for first two levels
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
        SerialPuts("\nGreat Work! Level passed! Preparing next level...\n"); //message to serial.
        HAL_Delay(5000); //show for 5 seconds.
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);   // turn off LED
        iteration_num++;
        success = level(iteration_num);
    } // after third level occurs, while loop doesn't run. But if was success, ends game and unlocks the "lock".
    if (success) {
        SerialPuts("Congratulations. You passed all three levels and fully unlocked the lock! Get out before the lock locks you up again!");
        uint32_t now = HAL_GetTick();
        while ((HAL_GetTick()-now) < 900000) { //continue flashing the blinking LED of success for max. 15 min., users have the ability to disconnect the system or press reset button to their own wish at this point. 
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
        exit(0); //exit program automatically if nothing occurs within the 15 min.
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);   // turn off LED
        SerialPuts("Oh no! You were unable to break the lock. Hope to see you try again!");
        exit(0);   
    }
#endif

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

#ifdef SEVEN_SEGMENT
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // (remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!)
    
    Initialize7Segment();
    while (true)
        for (int i = 0; i < 10; ++i)
        {
            Display7Segment(i);
            HAL_Delay(1000);  // 1000 milliseconds == 1 second
        }
#endif

#ifdef KEYPAD_SEVEN_SEGMENT
    // Combines the previous two examples, displaying numbers from the keypad on the 7-segment display.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    Initialize7Segment();
    while (true)
    {
        int key = ReadKeypad();
        if (key >= 0)
            Display7Segment(keypad_symbols[key]-'0');  // tricky code to convert ASCII digit to a number
    }
#endif

#ifdef COLOR_LED
    // Cycle through all 8 possible colors (including off and white) as the on-board button is pressed.
    // This example assumes that the color LED is connected to pins D11, D12 and D13.

    // Remember that each of those three pins must go through a 220 ohm current-limiting resistor!
    // Also remember that the longest pin on the LED should be hooked up to GND.

    InitializePin(GPIOA, GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins
    while (true) {
        for (int color = 0; color < 8; ++color) {
            // bottom three bits indicate which of the three LEDs should be on (eight possible combinations)
            while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));   // wait for button press
            if(color%3 == 0) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, color & 0x04);  // blue  (hex 1 == 0001 binary)
            } else if (color%3 == 1) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, color & 0x06);  // green (hex 2 == 0010 binary)
            } else {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, color & 0x02);  // red   (hex 4 == 0100 binary)
            }
 
            while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button release
        }
    }
#endif

#ifdef ROTARY_ENCODER
    // Read values from the rotary encoder and update a count, which is displayed in the console.

    InitializePin(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;             // this gets incremented or decremented as we rotate the encoder

    while (true)
    {
        int delta = ReadEncoder(GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, &previousClk);  // update the count by -1, 0 or +1
        if (delta != 0) {
            count += delta;
            char buff[100];
            sprintf(buff, "%d  \r", count);
            SerialPuts(buff);
        }
        bool sw = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);  // read the push-switch on the encoder (active low, so we invert it using !)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, sw);  // turn on LED when encoder switch is pushed in
    }
#endif

#ifdef ANALOG
    // Use the ADC (Analog to Digital Converter) to read voltage values from two pins.

    __HAL_RCC_ADC1_CLK_ENABLE();        // enable ADC 1
    ADC_HandleTypeDef adcInstance;      // this variable stores an instance of the ADC
    InitializeADC(&adcInstance, ADC1);  // initialize the ADC instance
    // Enables the input pins
    // (on this board, pin A0 is connected to channel 0 of ADC1, and A1 is connected to channel 1 of ADC1)
    InitializePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, 0);   
    while (true)
    {
        // read the ADC values (0 -> 0V, 2^12 -> 3.3V)
        uint16_t raw0 = ReadADC(&adcInstance, ADC_CHANNEL_0);
        uint16_t raw1 = ReadADC(&adcInstance, ADC_CHANNEL_1);

        // print the ADC values
        char buff[100];
        sprintf(buff, "Channel0: %hu, Channel1: %hu\r\n", raw0, raw1);  // hu == "unsigned short" (16 bit)
        SerialPuts(buff);
    }
#endif

#ifdef PWM
    // Use Pulse Width Modulation to fade the LED in and out.
    uint16_t period = 100, prescale = 16;

    __TIM2_CLK_ENABLE();  // enable timer 2
    TIM_HandleTypeDef pwmTimerInstance;  // this variable stores an instance of the timer
    InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);   // initialize the timer instance
    InitializePWMChannel(&pwmTimerInstance, TIM_CHANNEL_1);          // initialize one channel (can use others for motors, etc)

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2); // connect the LED to the timer output

    while (true)
    {
        // fade the LED in by slowly increasing the duty cycle
        for (uint32_t i = 0; i < period; ++i)
        {
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
            HAL_Delay(5);
        }
        // fade the LED out by slowly decreasing the duty cycle
        for (uint32_t i = period; i > 0; --i)
        {
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
            HAL_Delay(5);
        }
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

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

uint8_t  UARTGetChar (uint32_t moduleInstance);
bool UARTHasChar(uint32_t moduleInstance);
bool UARTCanSend(uint32_t moduleInstance);
void initUART(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 config);
//My function prototypes

//My approach was to use the provided documents on canvas, and Professor Thweatt's recommendations to begin,
//I then went my own way towards the solution

int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";
    int phrase = 0;
    int i;

    // Stops the Watchdog timer.
    initBoard();
    const eUSCI_UART_ConfigV1 uartConfig =
    {
            EUSCI_A_UART_CLOCKSOURCE_SMCLK,
            312,
            8,
            0,
            EUSCI_A_UART_NO_PARITY,
            EUSCI_A_UART_LSB_FIRST,
            EUSCI_A_UART_ONE_STOP_BIT,
    };

    // TODO: Initialize EUSCI_A0
    initUART(EUSCI_A0_BASE, uartConfig);
    while(1)
    {
        rChar = UARTGetChar (EUSCI_A0_BASE);
        //checks if there is a character and grabs it


        if(rChar != -1)
        {
            while(!UARTCanSend(EUSCI_A0_BASE)); //waits for the buffer
            UART_transmitData(EUSCI_A0_BASE, rChar); //outputs the character to the terminal
        }


            if(rChar == '2')
                phrase = ONE;
            else if(rChar == '5' && phrase == ONE)
                phrase = TWO;
            else if(rChar == '3' && phrase == TWO)
                phrase = THREE;
            else if(rChar == '4' && phrase == THREE)
                phrase = COMPLETE;
            else
                phrase = 0;
            //My version of the state machine, I put it here because it worked for me at at the time

        if(phrase == COMPLETE) //only runs if the numbers are inputted in order
        {
            while(!UARTCanSend(EUSCI_A0_BASE));
            for(i = 0; i < 48; i++)
                UART_transmitData(EUSCI_A0_BASE, response[i]); //outputs each character in the *char string
            phrase = 0;
        }
    }
}

void initBoard()
{
    WDT_A_hold(WDT_A_BASE);
}

// TODO: FSM for detecting character sequence.
bool charFSM(char rChar)
{
    bool finished = false;

    if(rChar == '2' || rChar == '5' || rChar == '3' || rChar == '4')
        return true;

    return finished;
}

uint8_t  UARTGetChar (uint32_t moduleInstance) //grabs the character in the receipt buffer
{
    if(UARTHasChar(EUSCI_A0_BASE))
        return -1;
    return UART_receiveData(EUSCI_A0_BASE);
}

bool UARTHasChar(uint32_t moduleInstance)//checks to see if there is a character to grab
{
    if(!UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
        return false;
    return true;

}

bool UARTCanSend(uint32_t moduleInstance)//checks to see if it can even output the character
{
    if(!UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG))
        return false;
    return true;
}

void initUART(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 config) //initializes the UART
{
    UART_initModule(moduleInstance, &config);
    UART_enableModule(moduleInstance);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
}


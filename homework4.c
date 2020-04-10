#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "homework4.h"

uint8_t  UARTGetChar (uint32_t moduleInstance);
bool UARTHasChar(uint32_t moduleInstance);
bool UARTCanSend(uint32_t moduleInstance);



void initUART(uint32_t moduleInstance, const eUSCI_UART_ConfigV1 config)
{
    UART_initModule(moduleInstance, &config);
    UART_enableModule(moduleInstance);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN3,GPIO_PRIMARY_MODULE_FUNCTION);
}






int main(void)
{
    char rChar;
    char *response = "\n\n\r2534 is the best course in the curriculum!\r\n\n";

    // TODO: Declare the variables that main uses to interact with your state machine.
    char phrase = 0;

    // Stops the Watchdog timer.
    initBoard();
    // TODO: Declare a UART config struct as defined in uart.h.
    //       To begin, configure the UART for 9600 baud, 8-bit payload (LSB first), no parity, 1 stop bit.

    const eUSCI_UART_ConfigV1 uartConfig =
    {
            EUSCI_A_UART_CLOCKSOURCE_SMCLK,
            78,
            2,
            0,
            EUSCI_A_UART_NO_PARITY,
            EUSCI_A_UART_LSB_FIRST,
            EUSCI_A_UART_ONE_STOP_BIT,
    };



    // TODO: Initialize EUSCI_A0
    initUART(EUSCI_A0_BASE, uartConfig);
    //UART_ableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    while(1)
    {
        // TODO: Check the receive interrupt flag to see if a received character is available.
        //       Return 0xFF if no character is available.
        rChar = UARTGetChar (EUSCI_A0_BASE);
        // TODO: If an actual character was received, echo the character to the terminal AND use it to update the FSM.
        //       Check the transmit interrupt flag prior to transmitting the character.

        if(rChar != -1)
        {
            while(!UARTCanSend(EUSCI_A0_BASE));
            UART_transmitData(EUSCI_A0_BASE, rChar);
        }

        if(charFSM(rChar) && phrase != COMPLETE)
            phrase += rChar;

        if(phrase != ONE && phrase != TWO && phrase != THREE && phrase != COMPLETE)
            phrase = 0;


        // TODO: If the FSM indicates a successful string entry, transmit the response string.
        //       Check the transmit interrupt flag prior to transmitting each character and moving on to the next one.
        //       Make sure to reset the success variable after transmission.
        if(phrase == COMPLETE)
        {
            if(UARTCanSend(EUSCI_A0_BASE))
                UART_transmitData(EUSCI_A0_BASE, *response);
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

    if(rChar == 2 || rChar == 5 || rChar == 3 || rChar == 4)
        return true;

    return finished;
}

uint8_t  UARTGetChar (uint32_t moduleInstance)
{
    if(UARTHasChar(EUSCI_A0_BASE))
        return -1;
    return UART_receiveData(EUSCI_A0_BASE);

}

bool UARTHasChar(uint32_t moduleInstance)
{
    if(!UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
        return false;
    return true;

}

bool UARTCanSend(uint32_t moduleInstance)
{
    if(!UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG))
        return false;
    return true;

}

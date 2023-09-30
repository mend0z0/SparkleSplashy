
#include "p24fj256ga705.h"
#define FCY (4000000UL)
#include "libpic30.h"

// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PLLMODE = DISABLED       // PLL Mode Selection (No PLL used; PLLEN bit is not available)
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFCN = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config SOSCSEL = ON             // SOSC Power Selection Configuration bits (SOSC is used in crystal (SOSCI/SOSCO) mode)
#pragma config PLLSS = PLL_PRI          // PLL Secondary Selection Configuration bit (PLL is fed by the Primary oscillator)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration bit (Allow only one reconfiguration)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler bits (1:32,768)
#pragma config FWPSA = PR128            // Watchdog Timer Prescaler bit (1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT and SWDTEN disabled)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config WDTCMX = WDTCLK          // WDT MUX Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits (WDT uses LPRC)

// FPOR
#pragma config BOREN = OFF              // Brown Out Enable bit (Brown Out Disabled)
#pragma config LPCFG = OFF              // Low power regulator control (No Retention Sleep)
#pragma config DNVPEN = ENABLE          // Downside Voltage Protection Enable bit (Downside protection enabled using ZPBOR when BOR is inactive)

// FICD
#pragma config ICS = NONE               // ICD Communication Channel Select bits (Reserved, do not use)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FDEVOPT1
#pragma config ALTCMPI = DISABLE        // Alternate Comparator Input Enable bit (C1INC, C2INC, and C3INC are on their standard pin locations)
#pragma config TMPRPIN = OFF            // Tamper Pin Enable bit (TMPRN pin function is disabled)
#pragma config SOSCHP = ON              // SOSC High Power Enable bit (valid only when SOSCSEL = 1 (Enable SOSC high power mode (default))
#pragma config ALTI2C1 = ALTI2CDIS      // Alternate I2C pin Location (ASDA1 and ASCL1 on RB5 and RB6)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


/*******************/

#define MSG "HELLO! WHY IT'S LIKE THIS?! \r\n"


void _init_UART( void );
int SendByteOnUART( char data );
int SendDataOnUART(const char *p_data);

int main(){
    char data[7] = {0XAA, 0X77, 0X00, 0Xff, 0X33, 0X00, 0XAA};
    _init_UART();
    __delay_ms(5000);
    while(1){
        SendDataOnDMX( data, 7);
        __delay_ms(5);
    }
}

void _init_UART( void ){
    /*asm volatile (  "MOV #OSCCON, w1 \n"
                    "MOV #0x46, w2 \n"
                    "MOV #0x57, w3 \n"
                    "MOV.b w2, [w1] \n"
                    "MOV.b w3, [w1] \n"
                    "BCLR OSCCON, #6") ;*/
    //configure the GPIO pin RB7->TX, RB8->RX
    TRISBbits.TRISB7 = 0;   //  SET PINB.0 AS OUTPUT
    LATBbits.LATB7 = 1;     //  SET HIGH AT START UP
    RPOR3bits.RP7R = 3;    //   Assign U1TX To Pin RP7
    //configure the UART
    U1BRG = 0;             //  !!!!!!!!!!! SET IT TO 250K !!!!!!!!!!!Set the baud-rate to 9600 for Fcy = 4MHz.
    U1MODEbits.STSEL = 1;   // TWO STOP BITS!
    U1MODEbits.UARTEN = 1;  //  Enable UART peripheral.
    U1STAbits.UTXEN = 1;    //  Enable transmit
}

void SendBRKOnUART( int duration /*value in microseconds*/){
    // Disable UART
    U1STAbits.UTXEN = 0;
    // Set the GPIO to 0
    LATBbits.LATB7 = 0;
    // Wait for the desired duration
    __delay_us(duration);
    // Set the GPIO to 1
    LATBbits.LATB7 = 1;
    // Enable the UART
    U1STAbits.UTXEN = 1;  
}

int SendByteOnUART( char data ){
   //Waiting until the transmission ends
    while(U1STAbits.TRMT == 0){
    }
    //since the buffer is empty we upload new data;
    U1TXREG = data;
    return 1;
}

int SendDataOnUART(const char *p_data){
    char data = 1;
    while(data){
        data = (char)*p_data;
        SendByteOnUART(data);
        p_data += 1;
    }
    return 1;
}

int SendDataOnDMX( const char *p_data, int size){
    SendBRKOnUART(88);
    __delay_us(8);
    SendByteOnUART(0);
    for(int counter = 0; counter < size; ++counter){
       SendByteOnUART(*(p_data + counter)); 
    }
    return 1;
}
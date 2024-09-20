#include <stdint.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>

void GPIO_PORTF_Init(void);
void GPIO_PORTF_Interrupt_Init(void);
void GPIOPortF_Handler(void);

int main(void) {
    GPIO_PORTF_Init();               // Initialize GPIO
    GPIO_PORTF_Interrupt_Init();     // Setup interrupt for GPIO

    while (1) {
        // Main loop can be empty as interrupt handles everything
    }
}

// GPIO Initialization function (e.g., for PF4 and PF1)
void GPIO_PORTF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;  // Enable clock for Port F
    while ((SYSCTL_PRGPIO_R & 0x20) == 0) {};  // Wait for Port F to be ready

    GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock GPIO Port F
    GPIO_PORTF_CR_R = 0x1F;          // Allow changes to PF4-PF0

    GPIO_PORTF_DIR_R |= 0x02;        // Set PF1 (Red LED) as output
    GPIO_PORTF_DIR_R &= ~0x10;       // Set PF4 (Switch) as input

    GPIO_PORTF_DEN_R |= 0x12;        // Enable digital on PF1, PF4
    GPIO_PORTF_PUR_R |= 0x10;        // Enable pull-up resistor on PF4
}

// GPIO Interrupt Setup function
void GPIO_PORTF_Interrupt_Init(void) {
    GPIO_PORTF_IS_R &= ~0x10;        // PF4 is edge-sensitive
    GPIO_PORTF_IBE_R &= ~0x10;       // Interrupt is controlled by GPIOIEV
    GPIO_PORTF_IEV_R &= ~0x10;       // Falling edge trigger
    GPIO_PORTF_IM_R |= 0x10;         // Unmask interrupt for PF4

    NVIC_EN0_R |= (1 << 30);         // Enable interrupt in NVIC for Port F

    GPIO_PORTF_ICR_R |= 0x10;        // Clear any prior interrupt
}

// GPIO Interrupt Handler for Port F
void GPIOPortF_Handler(void) {
    if (GPIO_PORTF_RIS_R & 0x10) {   // Check if interrupt occurred on PF4
        GPIO_PORTF_DATA_R ^= 0x02;   // Toggle PF1 (Red LED)
        GPIO_PORTF_ICR_R |= 0x10;    // Clear interrupt flag for PF4
    }
}

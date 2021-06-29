/******************************************************************************
* File Name:   main.c
*
* Description: In this code example, OpAmp and the SAR ADC are configured to
*              operate in low power mode.
*              SAR ADC samples preconfigured set of readings and triggers an
*              interrupt to wake up the CPU. The DMA then transfers the data 
*              from FIFO to a local buffer. Once a specified number of 
*              samples are collected in the buffer, they are averaged and 
*              displayed on the UART. Device is put to deep sleep and
*              it wakes up on the next FIFO interrupt.
*
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* Number of Samples per FIFO level interrupt */
#define SAMPLES_PER_FIFO_LEVEL_INTR                          (40)

/* Size of local buffer*/
/* Size of FIFO configured is for 40 samples using design.modus file, every 5th
   iteration of FIFO Interrupt, the data stored in local buffer is averaged, i.e.
   40*5 = 200 is the maximum size of local buffer is used */
#define MAX_BUFFER_LENGTH                         (SAMPLES_PER_FIFO_LEVEL_INTR*5)

/* Channel 0 of DMA */
#define CHANNEL0                                  (0)

/*******************************************************************************
* Function Prototypes
********************************************************************************/
/* FIFO Interrupt Handler */
void fifo_interrupt(void);

/* DMA Interrupt Handler */
void dma_interrupt(void);

/* Function to calculate average of 40*5 samples of data acquired */
static uint16_t calculate_avg(uint16_t *);

/* Initialization functions */
static void analog_init(void);
static void aref_init(void);
static void sar_init(void);
static void dma_init(void);
static void timer_lp_oscillator_init(void);
static void opamp_init(void);

/*******************************************************************************
* Global Variables
********************************************************************************/
/* FIFO interrupt configuration structure */
/* Source is set to FIFO 0 and Priority as 7 */
const cy_stc_sysint_t FIFO_IRQ_cfg = {
    .intrSrc = (IRQn_Type) pass_interrupt_fifo_0_IRQn,
    .intrPriority = 7
};

/* DMA interrupt configuration structure */
/* Source is set to DW 0 and Priority as 7 */
const cy_stc_sysint_t intRxDma_cfg =
{
        .intrSrc      = cpuss_interrupts_dw0_28_IRQn,
        .intrPriority = 7
};

/* This flag set in the FIFO interrupt handler */
volatile bool fifoIntrTriggered = false;

/* This flag set in the DMA interrupt handler */
volatile bool dmaIntrTriggered = false;

/* DMA Transfer complete/error flags sent in DMA interrupt Handler*/
volatile uint8_t adc_dma_error;   /* ADCDma error flag */
volatile uint8_t adc_dma_done;    /* ADCDma done flag */

/* Buffer to store data from FIFO using DMA */
uint16_t sample_buffer[MAX_BUFFER_LENGTH] = {0};


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function for CM4 CPU. It does...
*    1. Initialize and enable Analog Reference, CTBm, SAR ADC and DMA block.
*    2. For 200 samples of data acquired, averages it and displays on UART.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Variable to keep count of valid DMA Interrupt */
    uint8_t dma_count = 0;

    /* Variable to store the average calculated from the samples */
    int16_t average = 0;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the debug uart */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                     CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Print message */
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("-----------------------------------------------------------\r\n");
    printf("PSoC 6 MCU: Low Power Analog Front End\r\n");
    printf("-----------------------------------------------------------\r\n\n");
    printf("Provide input voltage at pin P9.0. \r\n");
    printf("Current taken by the device can be measured from J13 header\r\n\n");

    /* Initialize Analog blocks */
    analog_init();

    /* Enable global interrupts */
    __enable_irq();

    /* Enable Timer */
    Cy_SysAnalog_TimerEnable(PASS);

    for (;;)
    {
        /* Wait till printf completes the UART transfer */
        while(cyhal_uart_is_tx_active(&cy_retarget_io_uart_obj) == true);

        /*
           Device enters deep sleep and wakes up with the level interrupt from FIFO.
           SAR ADC is configured to scan once every 5ms and FIFO interrupt is configured to
           trigger every 40 samples. So FIFO Interrupt occurs every 5ms*40 = 200ms.
           After the accumulation of 200 samples i.e. every 1 second, the average of
           200 samples is calculated, converted to milli-Volt equivalent and then sent
           to UART Terminal.
        */

        /* Enter Deep Sleep mode */
        Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);

        /* Check if the interrupt is from the FIFO */
        if(fifoIntrTriggered)
        {
            /* Clear the FIFO interrupt flag */
            fifoIntrTriggered = false;

            /* Wait for DMA transaction to complete*/
            while(!dmaIntrTriggered);

            /* Clear the DMA interrupt flag */
            dmaIntrTriggered = false;

            /* If error flag set from DMA Interrupt */
            if(adc_dma_error)
            {
                printf("DMA ERROR %d\r\n",adc_dma_error);

            }
            else
            {
                /* Increment DMA Interrupt Count */
                /* Every 40*5 samples, calculate average and send to UART */
                dma_count++;
                if(dma_count == 5)
                {
                    average = calculate_avg(sample_buffer);
                    printf("Average of %d Samples: %dmV\r\n",MAX_BUFFER_LENGTH, average);
                    dma_count=0;
                }

                /* Increment DMA Destination by 40, as 40 elements are acquired and transferred to buffer. */
                Cy_DMA_Descriptor_SetDstAddress(&cpuss_0_dw0_0_chan_28_Descriptor_0,
                                                sample_buffer+(SAMPLES_PER_FIFO_LEVEL_INTR*dma_count));
            }
        }
    }
}

/*******************************************************************************
* Function Name: analog_init
********************************************************************************
* Summary:
* This function calls initialization functions of analog blocks.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void analog_init(void)
{
    /* Initializes Analog Reference block */
    aref_init();

    /* Initializes and enables CTBM block */
    opamp_init();

    /* Initializes and enables SAR block */
    sar_init();

    /* Initializes and enables DMA block */
    dma_init();

    /* Initializes Timer and Low Power Oscillator */
    timer_lp_oscillator_init();
}

/*******************************************************************************
* Function Name: aref_init
********************************************************************************
* Summary:
* This function initializes and enables System Analog Reference Block.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void aref_init(void)
{
    cy_en_sysanalog_status_t result;

    /* Initialize AREF */
    result = Cy_SysAnalog_Init(&pass_0_aref_0_config);

    if (result == CY_RSLT_SUCCESS)
    {
        Cy_SysAnalog_Enable();
    }
}

/*******************************************************************************
* Function Name: opamp_init
********************************************************************************
* Summary:
* This function initializes and enables CTBm Block.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void opamp_init(void)
{
    cy_en_ctb_status_t result;

    /* Initialize CTBm- CTB0 */
    result = Cy_CTB_OpampInit(CTBM0, CY_CTB_OPAMP_0, &pass_0_ctb_0_oa_0_config);
    if(CY_CTB_SUCCESS == result)
    {
        Cy_CTB_Enable(CTBM0);
    }
}

/*******************************************************************************
* Function Name: sar_init
********************************************************************************
* Summary:
* This function initializes and enables SAR ADC Block, it also configures FIFO
* interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void sar_init(void)
{
    cy_en_sar_status_t result;

    /* Initialize the SAR ADC; it includes initialization of FIFO */
    result = Cy_SAR_Init(SAR0, &pass_0_saradc_0_sar_0_config);

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /*
       Initialize common resources for SAR ADCs in the pass block.
       Common resources include simultaneous trigger parameters, scan count
       and power up delay
    */
    result = Cy_SAR_CommonInit(PASS, &pass_0_saradc_0_config);

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable SAR block */
    Cy_SAR_Enable(SAR0);

    /* Enable the FIFO Level Interrupt mask */
    Cy_SAR_SetFifoInterruptMask(SAR0, CY_SAR_INTR_FIFO);

    /* Configure the interrupt and provide the ISR address. */
    (void)Cy_SysInt_Init(&FIFO_IRQ_cfg, fifo_interrupt);

    /* Enable the interrupt. */
    NVIC_EnableIRQ(FIFO_IRQ_cfg.intrSrc);
}
    
/*******************************************************************************
* Function Name: dma_init
********************************************************************************
* Summary:
* This function initializes and enables DMA Block. Also sets the DMA source and
* destination address.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void dma_init(void)
{
    cy_en_dma_status_t result;

    /* Initialize descriptor 0 */
    result = Cy_DMA_Descriptor_Init(&cpuss_0_dw0_0_chan_28_Descriptor_0,
                                             &cpuss_0_dw0_0_chan_28_Descriptor_0_config);
    if(CY_DMA_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    /* Initialize the channel and associate the descriptor to it */
    result = Cy_DMA_Channel_Init(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL,
                                          &cpuss_0_dw0_0_chan_28_channelConfig);
    if(CY_DMA_SUCCESS != result)
    {
        CY_ASSERT(0);
    }

    /* Get the pointer to RD_DATA register from the FIFO configuration structure
       to set it as the source address in the DMA */
    uint32_t *result_ptr = (uint32_t*)&(PASS_FIFO_RD_DATA(SAR0));

    /* Set DMA Source and Destination address */
    Cy_DMA_Descriptor_SetSrcAddress(&cpuss_0_dw0_0_chan_28_Descriptor_0, result_ptr);

    Cy_DMA_Descriptor_SetDstAddress(&cpuss_0_dw0_0_chan_28_Descriptor_0, sample_buffer);

    /*Set DMA Descriptor */
    Cy_DMA_Channel_SetDescriptor(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL,
                                  &cpuss_0_dw0_0_chan_28_Descriptor_0);

    /* Initialize and enable the interrupt from SAR DMA */
    Cy_SysInt_Init(&intRxDma_cfg, &dma_interrupt);
    NVIC_EnableIRQ((IRQn_Type)intRxDma_cfg.intrSrc);

    /* Enable DMA interrupt source. */
    Cy_DMA_Channel_SetInterruptMask(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL, CY_DMA_INTR_MASK);

    /* Enable DMA Channel and DMA Block to start descriptor execution process */
    Cy_DMA_Channel_Enable(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL);
    Cy_DMA_Enable(cpuss_0_dw0_0_chan_28_HW);
}

/*******************************************************************************
* Function Name: timer_lp_oscillator_init
********************************************************************************
* Summary:
* This function initializes deep sleep resources - Timer, Low power oscillator
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
static void timer_lp_oscillator_init(void)
{
    cy_en_sysanalog_status_t result;

    /* Initialize deep sleep resources - Timer, LPOSC */
    result = Cy_SysAnalog_DeepSleepInit(PASS, &cy_cfg_pass0_deep_sleep_config);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable Low-Power Oscillator */
    Cy_SysAnalog_LpOscEnable(PASS);

}

/*******************************************************************************
* Function Name: calculate_avg
********************************************************************************
* Summary:
* This function calculates the average of the sampled data.
*
* Parameters:
*  uint16_t* pointer to data samples stored in buffer.
*
* Return:
*  None
*
*******************************************************************************/
static uint16_t calculate_avg(uint16_t sample_data[])
{
    uint32_t result_sum = 0;

    /* Calculate the average of the 200 samples */
    for(uint16_t i=0; i<MAX_BUFFER_LENGTH; i++)
    {
        result_sum += sample_data[i];
    }
    result_sum /= MAX_BUFFER_LENGTH;

    /* Convert raw counts to milli-Volts */
    result_sum = Cy_SAR_CountsTo_mVolts(SAR0, CHANNEL0, result_sum);

    return (uint16_t)(result_sum);
}

/*******************************************************************************
* Function Name: dma_interrupt
********************************************************************************
* Summary:
* This function is the handler for DMA interrupt
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void dma_interrupt(void)
{
    dmaIntrTriggered = true;

    /* Check interrupt cause to capture errors. */
    if (CY_DMA_INTR_CAUSE_COMPLETION == Cy_DMA_Channel_GetStatus(cpuss_0_dw0_0_chan_28_HW,
                                        cpuss_0_dw0_0_chan_28_CHANNEL))
    {
        adc_dma_done = 1;
    }
    else if((CY_DMA_INTR_CAUSE_COMPLETION    != Cy_DMA_Channel_GetStatus(cpuss_0_dw0_0_chan_28_HW,cpuss_0_dw0_0_chan_28_CHANNEL)) &&
                                                (CY_DMA_INTR_CAUSE_CURR_PTR_NULL !=
                                                Cy_DMA_Channel_GetStatus(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL)))
    {
        /* DMA error occurred while ADC operations */
        adc_dma_error = Cy_DMA_Channel_GetStatus(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL);
    }

    /* Clear SAR DMA interrupt */
    Cy_DMA_Channel_ClearInterrupt(cpuss_0_dw0_0_chan_28_HW, cpuss_0_dw0_0_chan_28_CHANNEL);
 }


/*******************************************************************************
* Function Name: fifo_interrupt
********************************************************************************
* Summary:
* This function is the handler for FIFO interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void fifo_interrupt(void)
{

    /* Check for FIFO level interrupt */
    if(Cy_SAR_GetFifoInterruptStatus(SAR0) & CY_SAR_INTR_FIFO_LEVEL)
    {
        /* Set the FIFO interrupt triggered flag */
        fifoIntrTriggered = true;
    }

    /* Clear the FIFO Level, underflow and overflow interrupt*/
    Cy_SAR_ClearFifoInterrupt(SAR0, CY_SAR_INTR_FIFO_LEVEL | CY_SAR_INTR_FIFO_UNDERFLOW | CY_SAR_INTR_FIFO_OVERFLOW );

}

/* [] END OF FILE */

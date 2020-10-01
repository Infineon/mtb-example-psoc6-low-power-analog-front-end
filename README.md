# PSoC 6 MCU: Low-Power Analog Front End using OpAmp and SAR ADC

This example demonstrates the low-power analog features of PSoC® 6 MCU using an OpAmp and SAR ADC. This example is supported only for the devices like CY8C62x4 which have an OpAmp and SAR ADC capable of operating in System Deep Sleep mode. The analog components - OpAmp and SAR ADC - are configured to operate when the system is in deep sleep mode. In this example, the SAR ADC samples the input voltage provided at pin P9.0; once the number of samples in the FIFO becomes equal to the preconfigured number of samples, an interrupt is raised and the system switches to normal mode. The acquired data is moved from the FIFO to a buffer using DMA. Once a specified number of samples are collected in the buffer, they are averaged and displayed on the UART. 

## Requirements

- [ModusToolbox® software](https://www.cypress.com/products/modustoolbox-software-environment) v2.2  
- Board Support Package (BSP) minimum required version: 2.0.0  
- Programming Language: C  
- Associated Parts: PSoC 62S4 [PSoC® 6 MCU](http://www.cypress.com/PSoC6)

## Supported Toolchains (make variable 'TOOLCHAIN')

- GNU Arm® Embedded Compiler v9.3.1 (GCC_ARM) - Default value of `TOOLCHAIN`
- Arm compiler v6.11 (ARM)
- IAR C/C++ compiler v8.42.2 (IAR)

## Supported Kits (make variable 'TARGET')

- [PSoC 62S4 Pioneer Kit (CY8CKIT-062S4)](https://www.cypress.com/CY8CKIT-062S4) 

## Hardware Setup

This example uses the board's default configuration. See the kit user guide to ensure that the board is configured correctly.

## Software Setup

Install a terminal emulator if you don't have one. Instructions in this document use [Tera Term](https://ttssh2.osdn.jp/index.html.en).

This example requires no additional software or tools.

## Using the Code Example

### In Eclipse IDE for ModusToolbox:

1. Click the **New Application** link in the **Quick Panel** (or, use **File** > **New** > **ModusToolbox Application**). This launches the [Project Creator](http://www.cypress.com/ModusToolboxProjectCreator) tool.

2. Pick a kit supported by the code example from the list shown in the **Project Creator - Choose Board Support Package (BSP)** dialog.

   When you select a supported kit, the example is reconfigured automatically to work with the kit. To work with a different supported kit later, use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) to choose the BSP for the supported kit. You can use the Library Manager to select or update the BSP and firmware libraries used in this application. To access the Library Manager, click the link from the Quick Panel. 

   You can also just start the application creation process again and select a different kit.

   If you want to use the application for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. In the **Project Creator - Select Application** dialog, choose the example by enabling the checkbox.

4. Optionally, change the suggested **New Application Name**.

5. Enter the local path in the **Application(s) Root Path** field to indicate where the application needs to be created. 

   Applications that can share libraries can be placed in the same root path.

6. Click **Create** to complete the application creation process.

For more details, see the [Eclipse IDE for ModusToolbox User Guide](https://www.cypress.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox install directory}/ide_{version}/docs/mt_ide_user_guide.pdf*).

### In Command-line Interface (CLI):

ModusToolbox provides the Project Creator as both a GUI tool and a command line tool to easily create one or more ModusToolbox applications. See the "Project Creator Tools" section of the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) for more details.

Alternatively, you can manually create the application using the following steps.

1. Download and unzip this repository onto your local machine, or clone the repository.

2. Open a CLI terminal and navigate to the application folder.

   On Linux and macOS, you can use any terminal application. On Windows, open the **modus-shell** app from the Start menu.

   **Note:** The cloned application contains a default BSP file (*TARGET_xxx.mtb*) in the *deps* folder. Use the [Library Manager](https://www.cypress.com/ModusToolboxLibraryManager) (`make modlibs` command) to select and download a different BSP file, if required. If the selected kit does not have the required resources or is not [supported](#supported-kits-make-variable-target), the application may not work. 

3. Import the required libraries by executing the `make getlibs` command.

Various CLI tools include a `-h` option that prints help information to the terminal screen about that tool. For more details, see the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox install directory}/docs_{version}/mtb_user_guide.pdf*).

### In Third-party IDEs:

1. Follow the instructions from the [CLI](#in-command-line-interface-cli) section to create the application, and import the libraries using the `make getlibs` command.

2. Export the application to a supported IDE using the `make <ide>` command. 

    For a list of supported IDEs and more details, see the "Exporting to IDEs" section of the [ModusToolbox User Guide](https://www.cypress.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox install directory}/docs_{version}/mtb_user_guide.pdf*.

3. Follow the instructions displayed in the terminal to create or import the application as an IDE project.

## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector.

2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud.

3. Ensure the input voltage within range 0V-3.3V is provided at analog input pin P9.0. 

3. Program the board.

   - **Using Eclipse IDE for ModusToolbox:**

      1. Select the application project in the Project Explorer.

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**.

   - **Using CLI:**

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. You can specify a target and toolchain manually:
      ```
      make program TARGET=<BSP> TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TARGET=CY8CKIT-062S4 TOOLCHAIN=GCC_ARM
      ```

4. After programming, the application starts automatically. Ensure that the average of input voltage is displayed on the UART terminal.

    **Figure 1. Terminal Output on Program Startup**

    ![Terminal Output on Program Startup](images/terminal-output.png)

## Debugging

You can debug the example to step through the code. In the IDE, use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For more details, see the "Program and Debug" section in the [Eclipse IDE for ModusToolbox User Guide](https://www.cypress.com/MTBEclipseIDEUserGuide).

**Note:** **(Only while debugging)** On the CM4 CPU, some code in `main()` may execute before the debugger halts at the beginning of `main()`. This means that some code executes twice - before the debugger stops execution, and again after the debugger resets the program counter to the beginning of `main()`. See [KBA231071](https://community.cypress.com/docs/DOC-21143) to learn about this and for the workaround.

## Design and Implementation

In this example, the analog components (OpAmp and SAR ADC) are configured to operate when the system is in deep sleep mode to make this example consume extremely low-power. The OpAmp is configured as a voltage follower and its output is connected to the SAR ADC.

In deep sleep mode, the CPU and the rest of the peripherals are turned OFF, the SAR ADC and OpAmp continue to scan the inputs and load the results into the FIFO. The scan is initiated by the timer configured to trigger the SAR ADC every 5 ms. The interrupt from the FIFO wakes up the CPU when the number of samples in FIFO is equal to the preconfigured value. In this case, it is 40 samples. With the timer period of 5 ms, the CPU is woken up every 200 ms (40 x 5 ms). When an interrupt from the FIFO wakes up the CPU, the DMA transfers the raw data in the FIFO to local buffer. Every 200 samples i.e., every 1 second, the data in the local buffer is converted to millivolt equivalent, averaged, and sent to the UART terminal. 

All the hardware configuration described above is done using the device configurator. The configuration is saved in a custom design.modus file in the application. In the firmware, the hardware is accessed using the PDL (Peripheral Driver Library) rather than the higher-level HAL (Hardware Abstraction Library) because of  advanced features such as deep sleep operation of the SAR ADCs (with the availability of the Timer and the Low-Power Oscillator) which are not available in other PSoC 6 devices. This example uses deep sleep operation of the SAR ADC. 

**Figure 2. Flowchart**

![Flowchart](images/flowchart-sar-adc.png)

The timing diagram and average current calculation are shown in Figure 3.

**Figure 3. Timing Diagram and Current Calculation**

![Timing Diagram and Current Calculation](images/timing-diagram-sar-adc.png)

The current consumed by the PSoC 6 MCU device can be measured on the kit at the appropriate header. See the kit user guide for details. The bench current measurement for different states of the firmware is shown in Table 1. 

**Table 1. Current measurement**

| State  |  Average Current   |
| :------- | :------------    | 
| Analog resources configured but SAR ADC conversion not started. Device is put to System Deep Sleep mode | 7.4 uA |
| Normal operation | 75 uA |

### Resources and Settings

This code example uses a custom configuration defined in the *design.modus* file located in *COMPONENT_CUSTOM_DESIGN_MODUS*. There is a sub-folder for each board supported. The components used in this example are one SAR ADC, one CTBm, DMA and Timer.

The analog components used in this example are SAR ADC(SAR0) and OpAmp(CTBM0) which are available under the **Peripherals** tab in the Device Configurator. Because the OpAmp is configured to operate in deep sleep mode, the deep sleep clock and low-power oscillator are also enabled. The timer is used as the trigger for SAR ADC and configured to operate at 200 Hz, so the SAR ADC is triggered every 5 ms. The analog reference block (AREF) is configured to enable all voltages and current generators in deep sleep mode. 

**Figure 4. Enabling Analog Components in Device Configurator**

![Enabling Analog Components in Device Configurator](images/device-config-main-sar-adc.png)

In programmable analog, the OpAmp reference current is configured to the lowest option of 100 nA and the deep sleep option is enabled for the OpAmp.

**Figure 5. Enabling OpAmp Deep Sleep Operation**

![Enabling OpAmp Deep Sleep Operation](images/device-config-prog-analog-sar-adc.png)

The OpAmp is configured for low power and the connections are configured for unity gain configuration. It acts as an input buffer. 

**Figure 6. OpAmp Charge Pump Clock configuration**

OpAmp includes a charge pump to get rail-to-rail input. When opamp configured for operation in System Deep Sleep mode, to get rail-to-rail input the pump clock source should be set to Deep Sleep Clock.

![OpAmp Charge Pump Clock configuration](images/ctbm-clock-sar-adc.png)

**Figure 7. OpAmp voltage follower configuration**

![OpAmp voltage follower configuration](images/device-config-ctbm-config-sar-adc.png)

The input to the SAR ADC is configured to be the output of the OpAmp, and the clock source is set to the deep sleep clock. Only one channel of the SAR ADC is used in this example. The SAR ADC is configured as single-ended where the reference voltage VDDA gives the input range of 0V to VDDA and input sampling is set to unsigned, therefore input voltage range is 0V-3.3V. The FIFO is configured to hold 40 samples, exceeding which an interrupt is raised which triggers a DMA transfer. This is configured with the EOS trigger output option. 

**Figure 8. SAR ADC configuration**

![SAR ADC configuration](images/device-config-sar-config-sar-adc.png)

The DMA is configured to transfer 40 samples to a local buffer on every FIFO interrupt from the SAR ADC.

**Figure 9. DMA configuration**

![DMA configuration](images/device-config-dma-config-sar-adc.png)

Table 2 provides a summary of the drivers used in the code example.

**Table 2. Application Resources**

| Resource  |  Alias/Object     |    Purpose     |
| :------- | :------------    | :------------ |
| SAR  (PDL) | SAR          | SAR Driver to measure sensor voltages |
| SYSANALOG (PDL) | PASS    | SYSANALOG Driver for AREF, Timer and Deep Sleep Clock Configuration |
| CTB (PDL)  | CTBM | OpAmp for input buffer  |
| DMA (PDL)  | DMA | Data Transfer  |
| UART (HAL)| cy_retarget_io_uart_obj | UART HAL object used by Retarget-IO for Debug UART port  |

## Related Resources

| Application Notes                                            |                                                              |
| :----------------------------------------------------------- | :----------------------------------------------------------- |
| [AN228571](https://www.cypress.com/AN228571) – Getting Started with PSoC 6 MCU on ModusToolbox | Describes PSoC 6 MCU devices and how to build your first application with ModusToolbox |
| [AN221774](https://www.cypress.com/AN221774) – Getting Started with PSoC 6 MCU on PSoC Creator | Describes PSoC 6 MCU devices and how to build your first application with PSoC Creator |
| [AN210781](https://www.cypress.com/AN210781) – Getting Started with PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity on PSoC Creator | Describes PSoC 6 MCU with BLE Connectivity devices and how to build your first application with PSoC Creator |
| [AN215656](https://www.cypress.com/AN215656) – PSoC 6 MCU: Dual-CPU System Design | Describes the dual-CPU architecture in PSoC 6 MCU, and shows how to build a simple dual-CPU design |
| **Code Examples**                                            |                                                              |
| [Using ModusToolbox](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software) | [Using PSoC Creator](https://www.cypress.com/documentation/code-examples/psoc-6-mcu-code-examples) |
| **Device Documentation**                                     |                                                              |
| [PSoC 6 MCU Datasheets](https://www.cypress.com/search/all?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A575&f[2]=field_related_products%3A114026) | [PSoC 6 Technical Reference Manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f[0]=meta_type%3Atechnical_documents&f[1]=resource_meta_type%3A583) |
| **Development Kits**                                         | Buy at www.cypress.com                                       |
| [CY8CKIT-062S4](https://www.cypress.com/CY8CKIT-062S4) PSoC 62S4 Pioneer Kit | [CY8CKIT-062-BLE](https://www.cypress.com/CY8CKIT-062-BLE) PSoC 6 BLE Pioneer Kit | [CY8CKIT-062-WiFi-BT](https://www.cypress.com/CY8CKIT-062-WiFi-BT) PSoC 6 WiFi-BT Pioneer Kit |
| [CY8CPROTO-063-BLE](https://www.cypress.com/CY8CPROTO-063-BLE) PSoC 6 BLE Prototyping Kit | [CY8CPROTO-062-4343W](https://www.cypress.com/CY8CPROTO-062-4343W) PSoC 6 Wi-Fi BT Prototyping Kit |
| [CY8CKIT-062S2-43012](https://www.cypress.com/CY8CKIT-062S2-43012) PSoC 62S2 Wi-Fi BT Pioneer Kit | [CY8CPROTO-062S3-4343W](https://www.cypress.com/CY8CPROTO-062S3-4343W) PSoC 62S3 Wi-Fi BT Prototyping Kit |
| [CYW9P62S1-43438EVB-01](https://www.cypress.com/CYW9P62S1-43438EVB-01) PSoC 62S1 Wi-Fi BT Pioneer Kit | [CYW9P62S1-43012EVB-01](https://www.cypress.com/CYW9P62S1-43012EVB-01) PSoC 62S1 Wi-Fi BT Pioneer Kit |                                                              |
| **Libraries**                                                 |                                                              |
| PSoC 6 Peripheral Driver Library (PDL) and docs  | [mtb-pdl-cat1](https://github.com/cypresssemiconductorco/mtb-pdl-cat1) on GitHub |
| Cypress Hardware Abstraction Layer (HAL) Library and docs     | [mtb-hal-cat1](https://github.com/cypresssemiconductorco/mtb-hal-cat1) on GitHub |
| Retarget IO - A utility library to retarget the standard input/output (STDIO) messages to a UART port | [retarget-io](https://github.com/cypresssemiconductorco/retarget-io) on GitHub |
| **Middleware**                                               |                                                              |
| CapSense® library and docs                                    | [capsense](https://github.com/cypresssemiconductorco/capsense) on GitHub |
| Links to all PSoC 6 MCU Middleware                           | [psoc6-middleware](https://github.com/cypresssemiconductorco/psoc6-middleware) on GitHub |
| **Tools**                                                    |                                                              |
| [Eclipse IDE for ModusToolbox](https://www.cypress.com/modustoolbox)     | The cross-platform, Eclipse-based IDE for IoT designers that supports application configuration and development targeting converged MCU and wireless systems.             |
| [PSoC Creator™](https://www.cypress.com/products/psoc-creator-integrated-design-environment-ide) | The Cypress IDE for PSoC and FM0+ MCU development.            |

## Other Resources

Cypress provides a wealth of data at www.cypress.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC 6 MCU devices, see [How to Design with PSoC 6 MCU - KBA223067](https://community.cypress.com/docs/DOC-14644) in the Cypress community.

## Document History

Document Title: *CE230700 - PSoC 6 MCU - Low Power Analog Front End*

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | New code example      |

------

All other trademarks or registered trademarks referenced herein are the property of their respective owners.

![banner](images/ifx-cy-banner.png)

-------------------------------------------------------------------------------

© Cypress Semiconductor Corporation, 2020. This document is the property of Cypress Semiconductor Corporation and its subsidiaries ("Cypress"). This document, including any software or firmware included or referenced in this document ("Software"), is owned by Cypress under the intellectual property laws and treaties of the United States and other countries worldwide. Cypress reserves all rights under such laws and treaties and does not, except as specifically stated in this paragraph, grant any license under its patents, copyrights, trademarks, or other intellectual property rights. If the Software is not accompanied by a license agreement and you do not otherwise have a written agreement with Cypress governing the use of the Software, then Cypress hereby grants you a personal, non-exclusive, nontransferable license (without the right to sublicense) (1) under its copyright rights in the Software (a) for Software provided in source code form, to modify and reproduce the Software solely for use with Cypress hardware products, only internally within your organization, and (b) to distribute the Software in binary code form externally to end users (either directly or indirectly through resellers and distributors), solely for use on Cypress hardware product units, and (2) under those claims of Cypress's patents that are infringed by the Software (as provided by Cypress, unmodified) to make, use, distribute, and import the Software solely for use with Cypress hardware products. Any other use, reproduction, modification, translation, or compilation of the Software is prohibited.  
TO THE EXTENT PERMITTED BY APPLICABLE LAW, CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH REGARD TO THIS DOCUMENT OR ANY SOFTWARE OR ACCOMPANYING HARDWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. No computing device can be absolutely secure. Therefore, despite security measures implemented in Cypress hardware or software products, Cypress shall have no liability arising out of any security breach, such as unauthorized access to or use of a Cypress product. CYPRESS DOES NOT REPRESENT, WARRANT, OR GUARANTEE THAT CYPRESS PRODUCTS, OR SYSTEMS CREATED USING CYPRESS PRODUCTS, WILL BE FREE FROM CORRUPTION, ATTACK, VIRUSES, INTERFERENCE, HACKING, DATA LOSS OR THEFT, OR OTHER SECURITY INTRUSION (collectively, "Security Breach"). Cypress disclaims any liability relating to any Security Breach, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any Security Breach. In addition, the products described in these materials may contain design defects or errors known as errata which may cause the product to deviate from published specifications. To the extent permitted by applicable law, Cypress reserves the right to make changes to this document without further notice. Cypress does not assume any liability arising out of the application or use of any product or circuit described in this document. Any information provided in this document, including any sample design information or programming code, is provided only for reference purposes. It is the responsibility of the user of this document to properly design, program, and test the functionality and safety of any application made of this information and any resulting product. "High-Risk Device" means any device or system whose failure could cause personal injury, death, or property damage. Examples of High-Risk Devices are weapons, nuclear installations, surgical implants, and other medical devices. "Critical Component" means any component of a High-Risk Device whose failure to perform can be reasonably expected to cause, directly or indirectly, the failure of the High-Risk Device, or to affect its safety or effectiveness. Cypress is not liable, in whole or in part, and you shall and hereby do release Cypress from any claim, damage, or other liability arising from any use of a Cypress product as a Critical Component in a High-Risk Device. You shall indemnify and hold Cypress, its directors, officers, employees, agents, affiliates, distributors, and assigns harmless from and against all claims, costs, damages, and expenses, arising out of any claim, including claims for product liability, personal injury or death, or property damage arising from any use of a Cypress product as a Critical Component in a High-Risk Device. Cypress products are not intended or authorized for use as a Critical Component in any High-Risk Device except to the limited extent that (i) Cypress's published data sheet for the product explicitly states Cypress has qualified the product for use in a specific High-Risk Device, or (ii) Cypress has given you advance written authorization to use the product as a Critical Component in the specific High-Risk Device and you have signed a separate indemnification agreement.  
Cypress, the Cypress logo, Spansion, the Spansion logo, and combinations thereof, WICED, PSoC, CapSense, EZ-USB, F-RAM, and Traveo are trademarks or registered trademarks of Cypress in the United States and other countries. For a more complete list of Cypress trademarks, visit cypress.com. Other names and brands may be claimed as property of their respective owners.

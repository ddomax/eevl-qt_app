# PC Host Application for EasyExcel2USB Evaluation Tool
## Scope
This application runs on a PC with x64 architecture. It communicates with the VBA program through the clipboard. For interactive editing, it polls and monitors the clipboard with a fixed interval setting in the timer as a workaround for unwanted behavior of the clipboard callback function.
## Basic Functions
* Readback register values from target device
* Send values to an Excel workbook
* Receive values from an Excel workbook
* Write the registers on target device
* Export operation history and generate C code for offline configuration
## Principle of operation
In current version, it acts as a TCP Client to communicate with MicroBlaze application via ethernet and uses LibMPSSE to initiate SPI communication via USB2.0.
## Roadmap
### Current Version
* Use online PDF2Excel tool and provide full capacity of register control in Excel using micros coded by VBA.
* Support SPI and AXI devices (SPI supported via USB using FT2232H as a physical bridge, AXI supported on Xilinx FPGA via Ethernet with a TCP server running on Microblaze embedded processor)
### Next Version
* Export operation history and generate C code for offline configuration
* Generation of chip-by-chip programming framework
## Source Code Structure
./SPI   library tools to operate FTDI Chip, currently SPI is the only supported protocol

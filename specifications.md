Backscatter RFID Project


#​ Introduction
##​ RFID Background
Radio Frequency Identification (RFID) is a wireless communication scheme where a Reader transmits a strong signal towards a device (Tag) and the Tag responds by sending back data.  This communication from the Tag to Reader is accomplished by the Tag varying the impedance of its antenna.  The energy impinging on the Tag’s antenna is absorbed and reflected (“scattered”) in some proportion depending on the antenna’s impedance characteristics.


If the Tag changes its antenna impedance rapidly, the back-scattered energy will change also, resulting in the scattered wave having double-sideband modulation components in addition to the original frequency.  The Reader can detect these sidebands and demodulate the data that the Tag sent.


The most common method for modulating these back-scattered sidebands is to vary the frequency of the impedance changes between two frequencies.  Bits are therefore able to be assigned to each unique frequency.  In other words, the Tag switches the antenna impedance between two levels (usually open- / short-circuit) at frequency #1 or frequency #2 depending on the current bit to be sent.


Such a communication method requires extremely little power consumption on the Tag side of the link.  With careful design, the Tag can even extract enough energy from the incoming signal from the Reader to power itself and operate the antenna switch.


##​ Synchronous Serial Communication
The two most common interfaces to connect peripheral devices to a central processor both use serial data connections, SPI and I2C.  It is possible, and even somewhat common, to find devices which are compatible with both formats using the same pins.


* SPI : Serial Peripheral Interface bus
* I2C : Inter-Integrated-Circuit bus


The details and timing diagrams for each of these formats are easily found on the internet.


#​ Project Specifications
The project for ECE 429 is to design and layout an integrated circuit in the On Semiconductor C5N 0.5um CMOS process that implements the major subsystems of an RFID tag.  A complete design would be capable of transmitting arbitrary data on programmable backscatter channel frequencies in the 900 MHz ISM band and also possibly in the 2.4 GHz ISM band.


The IC will appear as a slave peripheral to a processor and be controlled through a serial data connection via the SPI and/or I2C protocols.


Specific design and implementation details are the 




##​ Processor interface specification
[BLOCK DIAGRAM]


Communication with the host processor is via a serial peripheral interface, SPI




The device for the project will have a combined SPI/I2C, with internal detection of the input protocol being used.


Command and data format is in a register read/write style architecture


* 8-bit command, read/write a register location
* Optional 8-bit data, if required by the command mnemonic 
   * In I2C mode, the second and following bytes are either send or receive data
   * In SPI mode, the send data is clocked in on the MOSI device pin, while the received data is clocked out of the device on the MISO pin of the device.


http://www.i2cchip.com/mix_spi_i2c.html


https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi


https://learn.sparkfun.com/tutorials/i2c


http://www.i2c-bus.org/


http://www.nxp.com/documents/user_manual/UM10204.pdf




Other links:
http://wavedrom.com/


http://www.timing-diagrams.com/






##​ PLL Frequency Synthesizer
[BLOCK DIAGRAM]


###​ Programmable dividers
An internal module of the chip forms the basis of a Phase-Locked Loop (PLL) frequency synthesizer with a fractional divider.  These registers are two unsigned 8-bit values M, and N.
​ Voltage-controlled oscillator
* Similar to the CD4046 IC’s VCO
​2.2.3​ Phase comparator
* Type-1 (XOR)
​2.2.4​ Loop filter
* External to the chip
​2.2.5​ Registers
* M
* N
* Control
   * Enable VCO
   * Reset dividers

##​ Antenna impedance switches
These switch various impedances in parallel with the antenna to vary its net impedance and thence backscatter magnitude/phase.

##​ Charge pump
Accepts antenna input and outputs semi-regulated DC.
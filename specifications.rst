Backscatter RFID Chip
========================
ECE 429 course project
------------------------

.. rubber: clean specifications.out

.. include:: gitversion.txt

.. sectnum::
    :depth: 3

.. target-notes::

:Authors: Dan White and contributors
:Date: |date|
:Version: |version|






.. contents:: Table of Contents


================================================================
Introduction
================================================================


---------------------------------------------
Terminology
---------------------------------------------
The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED",  "MAY", and "OPTIONAL" in this document are to be interpreted as described in [RFC-2119]_.

.. [RFC-2119] https://www.ietf.org/rfc/rfc2119.txt

NOTE: The .rst version of this document SHALL be considered the canonical version, the .pdf is merely a convenience.
Discrepancies MUST resolve to the .rst version.

---------------------------------------------
Radio-frequency identification
---------------------------------------------
Radio-frequency identification (RFID) is a wireless communication scheme where a Reader transmits a strong signal towards a device (Tag) and the Tag responds by sending back data.
This communication from the Tag to Reader is accomplished by the Tag varying the impedance of its antenna.
The energy impinging on the Tag’s antenna is absorbed and reflected (“scattered”) in some proportion depending on the antenna’s impedance characteristics.

If the Tag changes its antenna impedance rapidly, the back-scattered energy will change also, resulting in the scattered wave having double-sideband modulation components in addition to the original frequency.
The Reader can detect these sidebands and demodulate the data that the Tag sent.

The most common method for modulating these back-scattered sidebands is to vary the frequency of the impedance changes between two frequencies.
Bits are therefore able to be assigned to each unique frequency.
In other words, the Tag switches the antenna impedance between two levels (usually open- / short-circuit) at frequency #1 or frequency #2 depending on the current bit to be sent.

Such a communication method requires extremely little power consumption on the Tag side of the link.
With careful design, the Tag can even extract enough energy from the incoming signal from the Reader to power itself and operate the antenna switch.


------------------------------------
Synchronous Serial Communication
------------------------------------
The two most common interfaces to connect peripheral devices to a central processor both use serial data connections, SPI and I2C.
It is possible, and even somewhat common, to find devices which are compatible with both formats using the same pins.

* SPI : Serial Peripheral Interface bus
* I2C : Inter-Integrated-Circuit bus

The details and timing diagrams for each of these formats are easily found on the internet.



================================================================
Project Specifications
================================================================

The project for ECE 429 is to design and layout an integrated circuit in the On Semiconductor C5N 0.5um CMOS process that implements the major subsystems of an RFID tag.
A complete design would be capable of transmitting arbitrary data on programmable backscatter channel frequencies in the 915 MHz ISM band and also possibly in the 2.4 GHz ISM band.



-----------------------------------------
Processor interface specification
-----------------------------------------

The processor interface to this chip SHALL via a combined SPI / I2C slave port.
Slave device circuitry SHALL properly detect the beginning of either an I2C or SPI transaction and behave accordingly.

From the view of the controlling processor, the device is a bank of up to 128 registers of 8-bits each which may be written to or read from.
The chip datasheet MUST specify the implemented address locations and the meaning of reads and/or writes to those addresses.
Writes to an unimplemented address SHOULD have no effect.
Reads of unimplemented register addresses will return meaningless data and SHOULD be ignored by the controlling processor.

The chip's I2C device address MUST be within the range of valid addresses according to the I2C specification.
The least-significant bits of the address MAY be pin-programmable, i.e. zero or more pins MAY be used to set the last address bits while the prefix bits are hard-coded to some valid value.


.. table:: SPI register write transaction

    ============  ====================  ======================
    Pin              byte0              byte1
    ============  ====================  ======================
    Bit #:          ``76543210``        ``76543210``
    ``MOSI``       ``0<Raddr>``         ``<8-data>``
    ``MISO``       ``xxxxxxxx``         ``xxxxxxxx``
    ============  ====================  ======================


.. table:: SPI register read transaction

    ============  ====================  ======================
    Pin              byte0              byte1
    ============  ====================  ======================
    Bit #:          ``76543210``        ``76543210``
    ``MOSI``       ``1<Raddr>``         ``xxxxxxxx``
    ``MISO``       ``xxxxxxxx``         ``<8-data>``
    ============  ====================  ======================




* 8-bit command, read/write a register location
* Optional 8-bit data, if required by the command mnemonic 
   * In I2C mode, the second and following bytes are either send or receive data
   * In SPI mode, the send data is clocked in on the MOSI device pin, while the received data is clocked out of the device on the MISO pin of the device.




.. table:: I2C register write

    ============  ====================  ======================  =============
    Pin              byte0                   byte1               byte2
    ============  ====================  ======================  =============
    Bit #:          ``76543210``        ``76543210``            ``76543210``
    ``SDA``         ``<Daddr>0``        ``x<Raddr>``            ``<8-data>``
    ============  ====================  ======================  =============


.. table:: I2C register read

    ============  ====================  ======================  =============
    Pin              byte0                   byte1               byte2
    ============  ====================  ======================  =============
    Bit #:          ``76543210``        ``76543210``            ``76543210``
    ``SDA``         ``<Daddr>1``        ``x<Raddr>``            ``<8-data>``
    ============  ====================  ======================  =============



Protocol references
*******************

http://www.i2cchip.com/mix_spi_i2c.html


https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi


https://learn.sparkfun.com/tutorials/i2c


http://www.i2c-bus.org/


http://www.nxp.com/documents/user_manual/UM10204.pdf




Other links:
http://wavedrom.com/


http://www.timing-diagrams.com/






--------------------------------------
NCO Frequency Synthesizer
--------------------------------------
A numerically-controlled oscillator forms the basis of the programmable backscatter frequency control for both channel selection and frequency-shift-keying (FSK) modulation.

[BLOCK DIAGRAM]


.. figure:: fig/nco.png

    Numerically-controlled oscillator diagram.
    This one outputs two square waves which have a 90-degree phase shift.


### Programmable dividers
An internal module of the chip forms the basis of a Phase-Locked Loop (PLL) frequency synthesizer with a fractional divider.  These registers are two unsigned 8-bit values M, and N.
Voltage-controlled oscillator
* Similar to the CD4046 IC’s VCO
2.2.3 Phase comparator
* Type-1 (XOR)
2.2.4 Loop filter
* External to the chip
2.2.5 Registers
* M
* N
* Control
* Enable VCO
* Reset dividers

## Antenna impedance switches
These switch various impedances in parallel with the antenna to vary its net impedance and thence backscatter magnitude/phase.

## Charge pump
Accepts antenna input and outputs semi-regulated DC.

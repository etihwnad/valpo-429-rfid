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


.. raw:: latex

    \pdfpxdimen=1in % 1 DPI
    \divide\pdfpxdimen by 96

.. |pm| unicode:: 0xB1 .. plus-minus sign



.. contents:: Table of Contents

.. raw:: latex

    \listoffigures
    \listoftables


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
Switch mapper
--------------------------------------
The switch mapper translates the mode of operation (FSK, or QAM) into appropriate antenna switch states.
Switch states are translated as ``switch[x] = 0``: NMOS off, and ``switch[x] == 1``: NMOS on.

In FSK mode (``mode == 0``), the input *fmod* is directly passed to ``switch[0]`` while the other switches remain off.
For QAM mode (``mode == 1``), the 2-bit input *symbol[1:0]* determines which single switch is on and the *fmod* input is ignored.


.. figure:: fig/sym-switch.png
    :width: 80%

    Switch state mapping block diagram.  See the table "Symbol to antenna switch mapping table" for the decoding.


.. table:: Symbol to antenna switch mapping table.

    ======  ======  ============    =============
    mode    fmod    symbol[1:0]     switch[2:0]
    ======  ======  ============    =============
    ``0``   ``0``   ``XX``          ``000``
    ``0``   ``1``   ``XX``          ``001``
    ``1``   ``X``   ``00``          ``000``
    ``1``   ``X``   ``01``          ``001``
    ``1``   ``X``   ``10``          ``010``
    ``1``   ``X``   ``11``          ``100``
    ======  ======  ============    =============

------------------------------------------
Numerically-controlled oscillator (NCO)
------------------------------------------
A numerically-controlled oscillator forms the basis of the programmable backscatter frequency control for both channel selection and frequency-shift-keying (FSK) modulation.
Two N-bit frequency control words, *fcw0[N-1:0]* and *fcw1[N-1:0]*, are applied to a multiplexer whose output is selected by the state of *fsel*
The current state of the phase accumulator register and the selected frequency control word are added and used to set the next state of the phase accumulator register, causing the accumulator to increment its state by *fcw* at each clock cycle.
Only the most-significant bit of the phase accumulator is used as the output signal, which is then a square wave at an average frequency of:

.. math::

    f_{out} = \dfrac{\mathit{fcw}[:]}{2^N} f_{clk}

The smallest change in average output frequency for the NCO is given by:

.. math::

    f_{res} = \dfrac{f_{clk}}{2^N}

The duty cycle is not guaranteed to be 50\% -- the high and low times may vary by |pm| 1 clock period. See reference [WP-NCO] for more information about NCO output characteristics.

.. figure:: fig/nco.png
    :width: 80%

    Numerically-controlled oscillator diagram and signals.
    This one outputs two square waves which have a 90-degree phase shift.

.. [WP-NCO] https://en.wikipedia.org/wiki/Numerically_controlled_oscillator



------------------------------------------
Antenna switches
------------------------------------------
These switch various impedances in parallel with the antenna to vary its net impedance and thence backscatter magnitude/phase.

------------------------------------------
Charge pump
------------------------------------------
Accepts antenna input and outputs semi-regulated DC.


LUR7 is the [Lund University Formula Student Engineering] teams 2015 effort of
building a Formula Student car with internal combustion engine.

The electronics system is centred around three ATmega32M1 chips on our custom
made circuit boards. To ease the use of the ATmega32M1 and its peripheral
hardware a number of libraries have been written. The code is hosted at [Github]
and licensed under [GPLv3].

Documentation for the code can be generated using [doxygen], or read it [here].

This documentation covers the shared folder header_and_config.

When using the code here in documented include LUR7.h to each file of your
project; all LUR7 libraries are included through this file. By including LUR7.h
there is also no need to include [avr/io.h] as it too is automatically included.

For official documentation on the hardware capabilities of the ATmega32M1 see
[Atmel's product page]. See Atmel's' [webdoc] for further documentation on 
their products.

Runnig Linux or Mac the software stack for building the system consists of
[avr-gcc], [avr-libc] and [avrdude]. Avrdude needs patching to support the
ATmega32M1, for instructions on how to do this see the walkthrough at
[lucidarme]. Also see Atmel's [AVR Libc Reference Manual].

Windows is supported throught the [Atmel Studio] software. For more information
on this see their webpage.

To transfer the compiled code onto the ATmega32M1 a [AVRISP mkII] is used.

\author Simon Wrafter
\copyright GNU Public License v3.0


[Lund University Formula Student Engineering]: http://www.luracing.com/ "Official Team Homepage"

[Github]:		https://github.com/simonwrafter/LUR7					"LUR7 Github"
[GPLv3]:		http://www.gnu.org/copyleft/gpl.html					"GNU Public License v3"
[doxygen]:		http://www.stack.nl/~dimitri/doxygen/index.html			"Doxygen"
[here]:			http://wrafter.synology.me										"LUR7 code documentation"

[avr/io.h]:		http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__io.html	"AVR Libc Reference Manual"

[Atmel's product page]:	http://www.atmel.com/devices/ATMEGA32M1.aspx	"ATmega32M1 product page"

[webdoc]:		http://www.atmel.com/webdoc/							"AVR WebDoc"

[avr-gcc]:		https://gcc.gnu.org/wiki/avr-gcc						"gnu.org, avr-gcc"
[avr-libc]:		http://www.nongnu.org/avr-libc/							"nongnu.org, avr-libc"
[avrdude]:		http://www.nongnu.org/avrdude/							"nongnu.org, avrdude"
[lucidarme]:	http://www.lucidarme.me/?p=3301							"patching avrdude"
[AVR Libc Reference Manual]:	http://www.atmel.com/webdoc/AVRLibcReferenceManual/index.html	"atmel webdoc, avr-libc"

[Atmel Studio]:	http://www.atmel.com/microsite/atmel_studio6/			"Atmel Studio main page"

[AVRISP mkII]:	http://www.atmel.com/webdoc/avrispmkii/index.html		"AVRSPI mkII User Guide"

# WinKeyer FSK for MMTTY

This is a source code repository of WinKeyerMMTY FSK extension. It supports [WinKeyer](https://www.k1elsystems.com)
devices containing WinKeyr [firmware
revision 3.1](https://www.k1elsystems.com/WinKeyer_31.html) or later. Those devices are capable of keying RTTY FSK signal.

## How to Use

1. Copy **WinKeyer.fsk** file to the directory
containing your installation of MMTTY.
1. Select `WinKeyer` from the `PTT & FSK` section of the MMTTY `Tx` configuration tab.
1. When the extension starts, it will show a small config and status window. Select the COM port on
which your WinKeyer is running. 

Please note, the baud rate and the number of stop bits are taken from the MMTTY configuration settings.
Optionally, configure its additional settings using that window.

## How to Build

Borland C++ Builder version 5.0 is needed to build this project.

After a successful build, you will find `WinKeyerMMTY.dll`. Rename it to `WinKeyer.fsk`.

## History

This project is based on the tinyFSK project by Nobuyuki Oba JA7UDE, who, in turn,
based his work on the EXTFSK64 project.
There is still much unnecessary code, and the overall code structure is much too complicated.
Apologies from all contributors for a
rather dirty source code.

## Copyright and License

Copyright 2000-2022 Makoto Mori, Nobuyuki Oba, Rafal Lukawiecki

This file is part of WinKeyerMMTY FSK.

WinKeyerMMTTY FSK is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

WinKeyerMMTTY FSK is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with WinKeyer FSK for MMTTY,
see files COPYING and COPYING.LESSER. If not, see http://www.gnu.org/licenses/.

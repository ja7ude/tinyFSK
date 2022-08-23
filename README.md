# WinKeyer FSK for MMTTY

This is a source code repository of WinKeyerMMTY FSK extension project. It supports WinKeyer devices containing firmware
revision 3.1 or later. Those devices are capable of keying RTTY FSK signal.

Borland C++ Builder version 5.0 is needed to build this project.

After a successful build, you will have WinKeyerMMTY.dll. Rename it to WinKeyer.fsk and copy it to the directory
containing MMTTY. Subsequently, select WinKeyer from the "PTT & FSK" section of the MMTTY Tx configuration tab.

This project is based on the tinyFSK project by Nobuyuki Oba JA7UDE, who, in turn, based his work on the EXTFSK64 project.
There still remain unnecessary code and the overall code structure is much too complicated. Apologies from all contributors for a
rather dirty source code.

## Copyright and License

Copyright 2000-2022 Makoto Mori, Nobuyuki Oba, Rafal Lukawiecki

This file is part of WinKeyerMMTY FSK project.

WinKeyerMMTTY FSK is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation, either version 2.1 of the License, or (at your option) any later version.

WinKeyerMMTTY FSK is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with WinKeyer FSK for MMTTY, see file LICENSE.md.  If not, see
http://www.gnu.org/licenses/.

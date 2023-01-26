# Open License Service

The Open License Service is a part of the Open License ecosystem. It is designed to be a mediator between Open License Server and Open License Module.


## Supported operating systems
 - Linux(Ubuntu 22, Linux Mint 21)
 - Windows10, Windows11 
 
### Linux
Intall these to libraries.

`sudo apt install qt6-base-dev libqt6websockets6-dev libbotan-2-dev cmake build-essential`

Download project and navigete to folder with the Open License Service

`cd /path/to/OpenLicenseService`

Run cmake

`cmake .`

Make

`make`

Copy the application binary to a folder (e.g., /opt/OL/)

`cp src/OpenLicenseService /opt/OL/`

Set it up as service (there is a olsservice.service and setupService.sh script in deploy folder). Make sure, that the path to the application is set correctly in the service file!

`sudo /path/to/setupService.sh`

### Windows
install Qt6 with QtCreator

install MS Visual Studio 19 Tools

The most straightforward is to build the project with QT6 kit in release mode directly in Qt Creator IDE. For the demostration Qt 6.3.2 will be used, but feel free to substitute the version.

Copy the compiled binary to an empty folder.

Create an environment by running the command in command prompt

`C:\Qt\6.3.2\msvc2019_64\bin\qtenv2.bat`

use windeploy tool to deploy the application

`C:\Qt\6.3.2\msvc2019_64\bin\windeployqt.exe /path/to/OpenLicenseService.exe`

Copy botan.dll library to application directory

`cp /path/to/project/src/OpenLicenseCommunication/win64/botan.dll /path/to/folder/with/OpenLicenseService.exe/`


To setup the application as a service download [nssm](https://nssm.cc/).

Unzip the binary and run 

`/path/to/nssm.exe install <name of the service>`

Add application path to the Open License Service. Start the service in Windows services manager.





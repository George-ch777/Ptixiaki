#The functions of the smart home in the context of the internet of things

#Description
The main aim of this project is a system’s construction that will convert a simple house to a smart house,
with a small number of resources, environmentally friendly and affordable to the media level customer. 
For this reason, it is constructed a main control system that “is built” in Node-Red application. 
It provides basic house’s operations and allows to users to be informed and interact with the house’s operation. 
Additionally, the user has the possibility to be informed for “house’s condition” even when he is away from it, 
because all the data are sent in real time at user’s profile, in IBM platform. 
For this systems implementation they are used an ESP32, sensors and one Raspberry Pi, 
where are sent all the data from ESP32 via Wi-Fi and the communication protocol MQTT. 
Raspberry Pi has the role of the server and up on this the application operates safely. 
At the end, for the creation of the project’s scenarios, they were chosen the C programming languages, 
which was developed in Arduino application and JavaScript in function toy Node-Red. 

## Installation Arduino IDE

-Select the package (https://www.arduino.cc/en/Main/Donate) to install Arduino IDE.
-After you have downloaded and installed the latest version (remember it must be 1.6.4 or higher), you will need to start
the IDE and navigate to the Preferences menu. You can access it from the File menu in Windows or Linux, or the
Arduino menu on OS X.
We will be adding a URL to the new Additional Boards Manager URLs option. The list of URLs is comma separated,
and you will only have to add each URL once. New Adafruit boards and updates to existing boards will automatically
be picked up by the Board Manager each time it is opened. The URLs point to index files that the Board Manager uses
to build the list of available & installed boards.
To find the most up to date list of URLs you can add, you can visit the list of third party board URLs on the Arduino IDE
wiki (https://adafru.it/f7U). We will only need to add one URL to the IDE in this example, but you can add multiple URLS
by separating them with commas. Copy and paste the link below into the Additional Boards Manager URLs option in
the Arduino IDE preferences.
-Add the Adafruit Board Support package!
Paste
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
Into the "Additional Board Managers URLS" box

-Open your first project. To create a new project, select File ? New.
-Select your Arduino board. Go to Tools ? Board and select your board ? manage board ? download DOIT ESP32 DEVKIT V1
-Select your serial port. Go to Tools ? Serial Port menu
-Download the libraries. Go to Tools ? Manage libraries ? Download 1)DHTesp.h 2)Ticker.h 3)WiFi.h 4)PubSubClient.h 5)Thermocouple.h 6)MAX6675_Thermocouple.h
- Copy the sketch and download it to the board

## Running Node-Red on Raspberry Pi 

-Installing and Upgrading Node-RED. Running the following command
sudo apt install build-essential git
apt-get install nodered
-Running locally
node-red
-Autostart on boot
sudo systemctl enable nodered.service
-A Node-RED node for runing remote ssh command.
npm install node-red-contrib-ssh-client
-find the IP of raspberry. Use th IP scanner
-open a browser and write your IP:1880
-copy the flows and deploy
-open IP:1880/ui

## Create an account (IBM Watson) https://internetofthings.ibmcloud.com/
follow the steps https://cloud.ibm.com/docs/watson?topic=watson-about

# tsandu-automation

arduino-cli core list
arduino-cli core install esp8266:esp8266

arduino-cli lib update-index
arduino-cli lib list
arduino-cli lib search https | grep ESP

arduino-cli board list
arduino-cli board listall


arduino-cli -v compile --fqbn esp8266:esp8266:nodemcu simple/Proximity/Proximity.ino
arduino-cli -v upload  --fqbn esp8266:esp8266:nodemcu --port /dev/ttyUSB0 simple/Proximity/Proximity.ino

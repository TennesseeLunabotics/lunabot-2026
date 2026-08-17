Code written by omgeometry with AI assistance.

To be deployed to esp32 via
``` 
arduino-cli compile \
  --fqbn esp32:esp32:esp32 \
  /home/lunabot/lunabot-2026/esp_32
```
and 
```
  arduino-cli upload \
  -p /dev/ttyUSB0 \
  --fqbn esp32:esp32:esp32 \
  /path/to/your/project
```

Debug serial with 
```
arduino-cli monitor -p /dev/ttyUSB0 -c baudrate=9600
```
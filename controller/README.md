Code written by omgeometry with AI assistance. Can be launched with 
```
python -m http.server 8080 -d /home/lunabot/lunabot-2026/controller
```
in one terminal on the lunabot and 
```
python /home/lunabot/lunabot-2026/controller/web_joy_bridge.py
```
on another. This starts the server that can be connected to via any browser connected to the same wifi as the lunabot at http://<lunabot-ip>:8080/joystick.html (i.e http://lunabot.local:8080/joystick.html if over router or hotspot).

Does not start the entire robot. Only starts a joy node on the robot connected to inputs from the webpage. Launch file must still be run separately.
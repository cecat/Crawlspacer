# Crawlspacer
Monitor crawlspace temp and other things at a remote home (avoid pipes freezing, etc.)

This code sends a temperature reading every 5 minutes.  There are values set in the code
to monitor a crawlspace to raise a warning if the temperature is falling toward 32F
and another warning if the temperature falls to 32F or below. 

It uses MQTT to send these notices and warnings to Home Assistant, where you can set
up automations based on the topics as well as monitor temperature over time.

To use this code you will need to set either the hostname or IP address of your HA system,
which for this code we assume is running the Mosquito MQTT broker.
Also, when setting up Mosquitto MQTT you will have created an MQTT username and password -
those will also need to be set.

The best way to do this is to set up a file "secrets.h" that looks like this if you are
using a hostname:

```
const char *HA_USR = "your_HA_mqtt_username";
const char *HA_PWD = "your_HA_mqtt_passwd";
#define MYSERVER "your.server.tld" 

```

If you are going to use the IP address of your server instead then rather than the #define
line in secrets.h you should put the following, substituting your IP address 
(w.x.y.z) components for w, x, y, and z:

`byte server[] = { w, x, w, z };`

Alternatively, you can comment out the "#include secrets.h" line and fill this information in
following the instructions here:

```
const char *HA_USR = "your_HA_mqtt_username";
const char *HA_PWD = "your_HA_mqtt_passwd";
#define MYSERVER "my.server.com" // if using hostname
// _OR_
byte server[] = { x, x, x, x };  // if using IP address
```

You can take the .ino file in the src directory and paste it into the Particle.io web IDE,
make the above customizations (including adding a secrets.h file if you go that route),
then add the libraries (see project.properties for
the libraries. (note if you are reading this a long time from the last update and
having trouble it could be that something has changed in the libraries, so you
can check the version number in the project.properties file as well).

Of course you could also use this code to monitor the temperature of other things that
in cases where you want to know if a threshold has been reached.



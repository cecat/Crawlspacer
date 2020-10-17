# Crawlspacer
Monitor crawlspace temp and other things at a remote home (avoid pipes freezing, etc.)

This code sends a temperature reading every 5 minutes.  There are values set in the code
to monitor a crawlspace to raise a warning if the temperature is falling toward 32F
and another warning if the temperature falls to 32F or below. 

It uses MQTT to send these notices and warnings to Home Assistant, where you can set
up automations based on the topics as well as monitor temperature over time.
You will need to fill in the domain name or IP address of your HA system, which for this code
we assume is running the Mosquito MQTT broker.  When setting up Mosquitto MQTT you will have
created an MQTT username and password - those will also need to be entered into the code.  

You can take the .ino file in the src directory and paste it into the Particle.io web IDE,
but be sure to also add in the libraries (see project.properties for the libraries. (note
if you are reading this a long time from the last update and having trouble it could be
that something has changed in the libraries, so you can check the version number in
the project.properties file as well).

Of course you could also use this code to monitor the temperature of other things that
in cases where you want to know if a threshold has been reached.



# Crawlspacer
Monitor crawlspace temp and other things at a remote home (avoid pipes freezing, etc.)

This code sends a temperature reading every 5 minutes.  There are values set in the code
to monitor a crawlspace to raise a warning if the temperature is falling toward 32F
and another warning if the temperature falls to 32F or below. 

It uses MQTT to send these notices and warnings to Home Assistant, where you can set
up automations based on the topics as well as monitor temperature over time.

Of course you could also use this code to monitor the temperature of other things that
in cases where you want to know if a threshold has been reached.



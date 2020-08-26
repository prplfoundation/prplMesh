
# Directory Content
This diectory contains prplMesh utilities to work with hostapd
It has few features as listed below

## Configuration
Manipulates hostapd configuration.
* load
* save
* add
* remove
* edit
* disable
* find

## Process (yet to implement)
Manages hostapd process
* start
* stop
* restart

## Commands (yet to implement) 
Manages sending and receiving commands to and from ostapd

## Events (yet to implement)
Manages events originated by hostapd itself

# Code

## namespace
The entire code is under prplmesh::hostapd
* Configuration: there is a class named Configuration
* Process:       prplmesh::hostapd::process   (yet to implement)
* Commands:      prplmesh::hostapd::commands  (yet to implement)
* Events:        prplmesh::hostapd::event     (yet to implement)

## Limitations
We are updating hostapd's configuration files with the info that we get from 
the controller. Once we changed the configuration we send UPDATE command to hostapd 
making it re-read the configuration and updating the VAPs accordingly.

With that approach there are the following limitations:
 - all the VAPs are expected to be always present in the config, but to be
   commented out if not active.
 - any configuration change causing the hostapd config files to be re-generated
   and the hostapd processes restarted will override the prplMesh configuration.

## hostapd Configuration Format

hostapd has a special format that is NOT an ini like format.
below, between /// BEGIN hostapd.conf /// and /// END hostapd.conf /// is the 
format of the file.
note: the string "bss=" may be replaced by the
user in the call to load() with another vap-indicator (e.g. "interface=")

/// BEGIN hostapd.conf ///  

\### "head" part

\# everything until the first `bss=` in the file
\# is considered "head"
\# after the first `bss=` "vaps" are presented.
\# so we expect no parametrs that does not belong to vaps
\# after the first `bss=`
\# take a look below for more details
\# 
\# note that we don't expect a space between the key and the equal sign
\# the code in seeks for `key=` when a key is needed.
\# therefore `key =` (with space) will fail
\# also, everything immidiatly after the equal sign (=) is 
\# part of the value. the space before 11 in this line is part of the value:
bassid= 11:22:33:44:55:66
key=value 

\### "vaps part - we expect at least one vap to be configured ##

\# vap (bss and vap are interchangable) 
bss=wlan0_0

\# this key (ssid) belongs to the previous vap (bss value which is wlan0_0)
ssid=test2

# another vap 
bss=wlan0_1

# this key (bssid) belongs to the previous vap wlan0_1
bssid=00:13:10:95:fe:0b

///// END hostapd.conf ///


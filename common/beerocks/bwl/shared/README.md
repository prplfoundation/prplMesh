# Shared files directory

The `shared` directory contains classes shared among all the implementations that use actual WiFi radios (i.e.: the DWPAL and NL80211 flavors but not the Dummy one).

Currently, these classes provide the following functionality:
- Support for NL80211 standard commands through a NL80211 socket. 

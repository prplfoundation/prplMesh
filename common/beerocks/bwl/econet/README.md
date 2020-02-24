# BWL Dummy

Dummy BWL implementation for Linux (not using actual WiFi radios).

## Simulating Events

The BWL Dummy implementation provides simple events simulation support using text file modification, specifically for simulated client connect - disconnect.

The way this is implemented is using `inotify` support in Linux to watch a file named EVENT in each radio agent:

```bash
/tmp/$USER/beerocks/wlan0/EVENT
/tmp/$USER/beerocks/wlan2/EVENT
```

The dummy BWL implementation reads the first line of the EVENT file each time it is modified (or created), and parse the event, then act on it.

This allows simulating WLAN events simply by writing the event as it would have been received from hostapd into this file - according to  [upstream hostapd](https://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html).
For example, simulating client connected event to the 2.4G radio (wlan0):

```bash
echo "AP-STA-CONNECTED 11:22:33:44:55:66" > /tmp/$USER/beerocks/wlan0/EVENT
```

This results with the event picked by the BWL dummy implementation:

```bash
INFO 11:42:21:311 <140309100766976> ap_manager_thread.cpp[909] --> STA_Connected mac = 11:22:33:44:55:66
TRACE 11:42:21:311 <140309101423488> son_slave_thread.cpp[2509] --> received ACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION
INFO 11:42:21:311 <140309101423488> son_slave_thread.cpp[2520] --> client associated sta_mac=11:22:33:44:55:66
```

Then in the controller:

```bash
INFO 11:42:21:312 <140688433724288> son_master_thread.cpp[2451] --> client associated, mac=11:22:33:44:55:66 hostap mac=00:11:22:33:00:10 setting to channel=1 
```

Which can be seen in `bml_conn_map`:

```bash
beerocks_cli -c bml_conn_map

bml_connect: return value is: BML_RET_OK, Success status
bml_nw_map_query: return value is: BML_RET_OK, Success status

GW_BRIDGE: name: GW_MASTER, mac: 00:11:22:33:00:00, ipv4: 172.18.0.234
    ETHERNET: mac: 00:11:22:33:00:01
    RADIO: wlan0 mac: 00:11:22:33:00:10, ch: 1, bw: 80L, freq: 2412MHz
        fVAP[0]: wlan0.0 bssid: 00:11:22:33:00:10, ssid: N/A
            CLIENT: mac: 11:22:33:44:55:66, ipv4: 0.0.0.0, name: N/A, ch: 1, bw: 80L, rx_rssi: -127
    RADIO: wlan2 mac: 00:11:22:33:00:20, ch: 149, bw: 80L, freq: 5745MHz
        fVAP[0]: wlan2.0 bssid: 00:11:22:33:00:20, ssid: N/A
bml_disconnect: return value is: BML_RET_OK, Success status
```

Using this new capability in DUMMY mode, development of flows involving stations such as steering can be tested more thoroughly. In addition, this provides the basis for Agent support in DUMMY mode.

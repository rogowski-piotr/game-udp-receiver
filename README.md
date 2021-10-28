# GAME UDP RECIVER
This code is WIP. Requires the development of parsing received data.

# 1. Description
Allow to recive data from F1 2021 on every platforms. Program receives UDP datagrams with telemetry in game.

# 2. Data parsing 
See [official documentation](https://forums.codemasters.com/topic/80231-f1-2021-udp-specification/?_fromLogin=1) to understand reciving data.

# 3. Software setup
#### set local network access
```cpp
const char ssid[] = "HERE_YOUR_SSID";
const char pass[] = "HERE_YOUR_PASSWORD";
```
#### set the listening udp port
```cpp
unsigned const int localPort = 20777;
```

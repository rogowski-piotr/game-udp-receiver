# GAME UDP RECIVER
This code is WIP. Requires the development of parsing received data.

# 1. Description
Allow to recive data from F1 2019 on every platforms. The program is not responsible for displaying data on 
peripherial devices like lcd screen or led strips.

# 2. Data parsing 
See [this](https://forums.codemasters.com/topic/44592-f1-2019-udp-specification/) to understand reciving data.

# 3. Setup
#### set local network access
```cpp
const char ssid[] = "HERE_YOUR_SSID";
const char pass[] = "HERE_YOUR_PASSWORD";
```
#### set the listening udp port
```cpp
unsigned const int localPort = 5151;
```

# ip-indicator
a simple combination of programs showing the status of the server's assigned internal ip with 3 leds.

---
## ledmon
small program running on arduino atmega328p(the UNO) to visualize no ip/wrong ip/correct ip. on failure to get info from machine it will go through all colors until it gets info.
### pre-setup
- connect red led to port 9 of your UNO, this will show 'no ip' status
- connect yellow led to port 10 of your UNO, this will show 'wrong ip' status
- connect green led to port 11 of your UNO, this will show 'correct ip' status

in theory it could all work with another arduino or similar board but you will have to do some porting here and there regarding the device preferences in the makefile and the ports defined in the main code.
### setup
1. `cd ledmon`
2. `make flash`
---
## ipcheck
small daemon to check if the internal ip assigned is correct, will send result of this info over serial to ledmon.
### setup
1. `cd ipcheck`
2. `make build`
3. `./ipcheck youriface wantedipaddress comport` e.g.: `./ipcheck eth0 192.168.0.104 /dev/ttyACM0`
### usage
`usage: ipcheck [iface] [wanted ip] [comport] (--no-daemon)`

it's advised to just run this as daemon in the background but the `--no-daemon` flag can be helpful in debugging configuration.

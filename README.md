# <p align="center">**dscvr**</p>
<p align="center">
Script that discovers devices on a given subnet. It works by sending an ARP request
to all IPs in the subnet and reads which ones reply. If flooding your network with ARP requests sounds bad, probably a good idea to avoid using this. </br><b>This only works on Linux!</b> </p>

# Build
```
make release
```
as a result you get **./dscvr** binary
# Usage
root privileges are required for creating a specific socket
```
sudo dscvr
```
# Example output

```
Available interfaces: 
(0): lo 
(1): eno1 
(2): docker0 
------------------------------ 
Choose an interface: 1 
Enter mask (0-31): 24 
Waiting for responses indefinitely, CTRL+C to stop 
Received: 
IP: 192.168.1.1  MAC: --mac--
IP: 192.168.1.100  MAC: --mac--
```
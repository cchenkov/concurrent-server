## Protocol

[packet length] [array size] [array of integers]

[packet length] - 4 bytes  
[array size] - 4 bytes  
[array of integers] - 1492 bytes

Max packet size = 1500 bytes

The length of the entire packet is stored in the first 4 bytes.  
The length of the array being sent is stored in the 4 bytes afterwards.  
The actual data can consist of up to 1496 bytes or 746 16-bit integers.


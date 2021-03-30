# tcpExchange
### Usage:

  `[HOST 1]: server <TCP PORT> <UDP PORT>`
  
 ` ...`
  
 `[HOST N]: client <mode(TCP|UDP)> <ip address> <port>`
  
  
  #### Example:
  
  
  `[HOST 1]: server 5001 5002`
  
  
  `[HOST 2]: client TCP 127.0.0.1 5001`
  
  
  `[HOST 3]: client UDP 192.168.0.101 5002`

"""
Does opening and closing many sockets leave sockets in the TIMED_WAIT state?
"""

import socket

addrs = socket.getaddrinfo ("kevinalbs.com", 80)
print (addrs)
ip = addrs[0][4][0]

for i in range(100):
    s = socket.create_connection (("kevinalbs.com", 80))
    s.shutdown (socket.SHUT_WR)


print ("netstat -q | Select-String -pattern %s" % ip)
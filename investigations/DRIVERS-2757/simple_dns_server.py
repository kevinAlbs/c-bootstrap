"""
A simplified DNS server in Python to test DRIVERS-2757.

To run:
    python simple_dns_server.py

To test:
    dig -p 5053 SRV _mongodb._tcp.test1.test.build.10gen.cc.

Inspired by https://gist.github.com/pklaus/b5a7876d4d2cf7271873
"""

import argparse
import sys
import traceback
import socketserver
from dnslib import DNSRecord, DNSHeader, QTYPE, RR, SRV, A
import time

TTL = 60 * 5
failhalf = False
failcounter = 0
sleep_before_responding = False

def make_dns_response(data):
    request = DNSRecord.parse(data)
    print("---- Got request:\n{}".format(request))
    reply = DNSRecord(DNSHeader(id=request.header.id, qr=1, aa=1, ra=1), q=request.q)

    qname = request.q.qname
    qn = str(qname)
    qtype = request.q.qtype
    qt = QTYPE[qtype]

    # Respond to only a few hard-coded DNS queries.
    if qn == "localhost.test.build.10gen.cc." and qt == "A":
        rdata=A("127.0.0.1")
        reply.add_answer(RR(rname=qname, rtype=QTYPE.A, rclass=1, ttl=TTL, rdata=rdata))
    if qn == "_mongodb._tcp.test1.test.build.10gen.cc." and qt == "SRV":
        rdata=SRV(priority=1, weight=1, port=27017, target="localhost.test.build.10gen.cc.")
        reply.add_answer(RR(rname=qname, rtype=QTYPE.SRV, rclass=1, ttl=TTL, rdata=rdata))
        rdata=SRV(priority=1, weight=1, port=27018, target="localhost.test.build.10gen.cc.")
        reply.add_answer(RR(rname=qname, rtype=QTYPE.SRV, rclass=1, ttl=TTL, rdata=rdata))

    print("---- Sending reply:\n{}".format(reply))

    return reply.pack()



class UDPRequestHandler(socketserver.BaseRequestHandler):

    def get_data(self):
        return self.request[0].strip()

    def send_data(self, data):
        return self.request[1].sendto(data, self.client_address)

    def handle(self):
        global failhalf, failcounter, sleep_before_responding
        try:
            failcounter += 1
            if failhalf and failcounter % 2 == 0:
                # Do not reply.
                print ("Failing request")
                return
            if sleep_before_responding:
                print ("Sleeping.")
                time.sleep (10)
            data = self.get_data()
            response = make_dns_response(data)
            self.send_data(response)
        except Exception:
            traceback.print_exc(file=sys.stderr)

def main():
    global failhalf, sleep_before_responding
    parser = argparse.ArgumentParser(description='Start a DNS implemented in Python.')
    parser.add_argument('--port', default=5053, type=int, help='The port to listen on.')
    parser.add_argument('--fail-half', default=False, action="store_true", help='When true, fail every other request.')
    parser.add_argument('--sleep', default=False, action="store_true", help='When true, sleep for 10 seconds before responding to request.')
    args = parser.parse_args()
    failhalf = args.fail_half
    sleep_before_responding = args.sleep
    print("Starting nameserver...")
    server = socketserver.ThreadingUDPServer(('', args.port), UDPRequestHandler)
    server.serve_forever()

if __name__ == '__main__':
    main()

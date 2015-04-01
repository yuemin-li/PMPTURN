#! /usr/bin/python
# Provide port mapping in a specific range for TURN server behind symetric NATs.
# The range is start inclusive and end exclusive.
# Author: yuemin

import miniupnpc
import sys

def initUPnP():

	u = miniupnpc.UPnP()
	print 'Initializing object...'
	print 'discover delay: ', u.discoverdelay
	print 'our ip address on the LAN: ', u.lanaddr
	u.discoverdelay = 200
	print 'discover delay set to ', u.discoverdelay
	print u.discover(), ' device(s) detected.'
	
	try:
		u.selectigd()
	except Exception, e:
		print 'Exception: ', e
		sys.exit(1)

	print 'Successful discover!'
	print 'Local IP address: ', u.lanaddr
	print 'External IP address: ', u.externalipaddress()
	print u.statusinfo()
	print u.connectiontype()

	return u

def addportmappingrange(u, start, end, proto):
	for port in range(start, end):
		if (u.addportmapping(port, proto, u.lanaddr, port, "TURN port mapping reserved.", '')):
			print 'port mapped from %u to %u.' % (port, port)

	return 0

def delportmappingrange(u, start, end, proto):
	for port in range(start, end):
		if (u.deleteportmapping(port, proto)):
			print "port mapping from %u to %u is deleted." % (port, port)
	return 0
	
# test function
def main():
	u = initUPnP() 
	addportmappingrange(u, 6666, 6669, 'UDP')
	delportmappingrange(u, 6666, 6668, 'UDP')
	

if __name__ == "__main__":
	main()



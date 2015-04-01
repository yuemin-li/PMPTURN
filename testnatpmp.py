#!/usr/bin/env python

from natpmplib import *


def main ():
	sock = natpmp()
	response = natpmpresp()
	initnatpmp(sock, '192.168.1.1')
	print sock.gateway
	#sendpublicaddressrequest(sock)
	#readnatpmpresponse(sock, response)
	#print response.publicaddr
	sendnewportmappingrequest(sock, 1, 6666, 9999, 500)
	readnatpmpresponse(sock, response)
	print response.privateport
	print response.mappedpublicport
			
	
	


if __name__ == "__main__":
	main()

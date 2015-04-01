#!/usr/bin/env python

import socket
import struct

NATPMP_PORT = 5351

class natpmp(object):
	s = 0
	gateway = ''
	has_pending_request = 0
	pending_request = []
	pending_request_len = 0 #request specific.
	pending_request_pack = ''
	try_number = 0
	retry_time = 0

'''
test = natpmp()
print test.s
print test.pending_request
'''

class natpmpresp(object):
	resptype = 0 #NATPMP_RESPTYPE_
	resultcode = 0 #NAT-PMP response code.
	epoch = 0 #Seconds since start of epoch. 
	publicaddr = ''
	privateport = 0
	mappedpublicport = 0
	lifetime = 0

'''
test2 = newportmapping()
print test2.resptype
print test2.mappedpublicport
'''

def initnatpmp(sock, forcedgw):
	sock.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
	sock.gateway = forcedgw
	sock.s.connect((sock.gateway, NATPMP_PORT)) #NATPMP_PORT=5351
	return 0


def closenatpmp(sock):
	socket.close(sock.s)
	return 0

def sendpendingrequest(sock):
	sock.s.send(sock.pending_request_pack, 0)
	return 0

def sendnatpmprequest(sock):
	sock.has_pending_request = 1
	sock.try_number = 1
	#sock.retry_time
	sendpendingrequest(sock)
	return 0

def getnatpmprequesttimeout(sock, timeout):
	pass

def sendpublicaddressrequest(sock):
	sock.pending_request.append(0)
	sock.pending_request.append(0)
	sock.pending_request_len = 2
	sock.pending_request_pack = struct.pack('BB', sock.pending_request[0], 
							sock.pending_request[1])
	return sendnatpmprequest(sock)

def sendnewportmappingrequest(sock, protocol, privateport, publicport, lifetime):
	sock.pending_request.append(0)
        sock.pending_request.append(protocol) # 1 for udp, 2 for tcp
        sock.pending_request.append(0)
        sock.pending_request.append(0)
        sock.pending_request.append(socket.htons(privateport)) #2bytes -> H
        sock.pending_request.append(socket.htons(publicport))  #2bytes -> H
	sock.pending_request.append(socket.htonl(lifetime))    #4bytes -> I
	sock.pending_request_pack = struct.pack('BBBBHHI', sock.pending_request[0], 
				sock.pending_request[1], sock.pending_request[2],
				sock.pending_request[3], sock.pending_request[4],
				sock.pending_request[5], sock.pending_request[6])
	return sendnatpmprequest(sock)

def readnatpmpresponse(sock, response):
	buf_size = 16
	rp = sock.s.recv(buf_size, 0)
	if (len(rp) == 12): #Public address response. OP Code = 128
		rp_unpack = struct.unpack('BBHII', rp)
		response.resultcode = rp_unpack[2]
		response.epoch = rp_unpack[3]
		response.publicaddr = rp_unpack[4]
	elif(len(rp) == 16): #New port mapping response. OP Code = 129 for UDP.
		rp_unpack = struct.unpack('BBHIHHI', rp)	
		response.resultcode = socket.ntohs(rp_unpack[2])
		response.epoch = socket.ntohl(rp_unpack[3])
		response.privateport = socket.ntohs(rp_unpack[4])
		response.mappedpublicport = socket.ntohs(rp_unpack[5])
		response.lifetime = socket.ntohl(rp_unpack[6])
	else:
		return -1

	return 0







		

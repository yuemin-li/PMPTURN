#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <winsock2.h>
#define snprintf _snprintf
#else
/* for IPPROTO_TCP / IPPROTO_UDP */
#include <netinet/in.h>
#endif

#include <miniupnpc/miniupnpc.h>


#define UPNPCOMMAND_SUCCESS (0)
#define UPNPDISCOVER_SOCKET_ERROR (-101)
#define UPNPDISCOVER_MEMORY_ERROR (-102)
#define UPNPCOMMAND_UNKNOWN_ERROR (-1)
#define UPNPCOMMAND_INVALID_ARGS (-2)
#define UPNPCOMMAND_INVALID_RESPONSE (-4)

int
UPNP_GetExternalIPAddress(const char * controlURL,
                          const char * servicetype,
                          char * extIpAdd);
int
UPNP_AddAnyPortMapping(const char * controlURL, const char * servicetype,
                       const char * extPort,
                       const char * inPort,
                       const char * inClient,
                       const char * desc,
                       const char * proto,
                       const char * remoteHost,
                       const char * leaseDuration,
                       char * reservedPort);

int
UPNP_AddPortMapping(const char * controlURL, const char * servicetype,
                    const char * extPort,
                    const char * inPort,
                    const char * inClient,
                    const char * desc,
                    const char * proto,
                    const char * remoteHost,
                    const char * leaseDuration);


const char * protofix(const char * proto)
{
        static const char proto_tcp[4] = { 'T', 'C', 'P', 0};
        static const char proto_udp[4] = { 'U', 'D', 'P', 0};
        int i, b;
        for(i=0, b=1; i<4; i++)
                b = b && (   (proto[i] == proto_tcp[i])
                          || (proto[i] == (proto_tcp[i] | 32)) );
        if(b)
                return proto_tcp;
        for(i=0, b=1; i<4; i++)
                b = b && (   (proto[i] == proto_udp[i])
                          || (proto[i] == (proto_udp[i] | 32)) );
        if(b)
                return proto_udp;
        return 0;
}

const char * strupnperror(int err)
{
        const char * s = NULL;
        switch(err) {
        case UPNPCOMMAND_SUCCESS:
                s = "Success";
                break;
        case UPNPCOMMAND_UNKNOWN_ERROR:
                s = "Miniupnpc Unknown Error";
                break;
        case UPNPCOMMAND_INVALID_ARGS:
                s = "Miniupnpc Invalid Arguments";
                break;
        case UPNPCOMMAND_INVALID_RESPONSE:
                s = "Miniupnpc Invalid response";
                break;
        case UPNPDISCOVER_SOCKET_ERROR:
                s = "Miniupnpc Socket error";
                break;
        case UPNPDISCOVER_MEMORY_ERROR:
                s = "Miniupnpc Memory allocation error";
                break;
        case 401:
                s = "Invalid Action";
                break;
        case 402:
                s = "Invalid Args";
                break;
        case 501:
                s = "Action Failed";
                break;
        case 606:
                s = "Action not authorized";
                break;
        case 701:
                s = "PinholeSpaceExhausted";
                break;
        case 702:
                s = "FirewallDisabled";
                break;
        case 703:
                s = "InboundPinholeNotAllowed";
                break;
        case 704:
                s = "NoSuchEntry";
                break;
        case 705:
                s = "ProtocolNotSupported";
                break;
        case 706:
                s = "InternalPortWildcardingNotAllowed";
                break;
        case 707:
                s = "ProtocolWildcardingNotAllowed";
                break;
        case 708:
                s = "WildcardNotPermittedInSrcIP";
                break;
        case 709:
                s = "NoPacketSent";
                break;
        case 713:
                s = "SpecifiedArrayIndexInvalid";
                break;
        case 714:
                s = "NoSuchEntryInArray";
                break;
        case 715:
                s = "WildCardNotPermittedInSrcIP";
                break;
        case 716:
                s = "WildCardNotPermittedInExtPort";
                break;
        case 718:
                s = "ConflictInMappingEntry";
                break;
        case 724:
                s = "SamePortValuesRequired";
                break;
        case 725:
                s = "OnlyPermanentLeasesSupported";
                break;
        case 726:
                s = "RemoteHostOnlySupportsWildcard";
                break;
        case 727:
                s = "ExternalPortOnlySupportsWildcard";
                break;
        default:
                s = "UnknownError";
                break;
        }
        return s;
}

static void SetRedirectAndTest(struct UPNPUrls * urls,
                   struct IGDdatas * data,
                   const char * iaddr,
                   const char * iport,
                   const char * eport,
                   const char * proto,
                   const char * leaseDuration,
                   const char * description,
                   int addAny)
{
    char externalIPAddress[40];
//    char intClient[40];
//    char intPort[6];
//    char reservedPort[6];
//    char duration[16];
    int r;

    if(!iaddr || !iport || !eport || !proto)
    {
        fprintf(stderr, "Wrong arguments\n");
        return;
    }
    proto = protofix(proto);
    if(!proto)
    {
        fprintf(stderr, "invalid protocol\n");
        return;
    }
    r = UPNP_GetExternalIPAddress(urls->controlURL,
                      data->first.servicetype,
                      externalIPAddress);
    if(r!=UPNPCOMMAND_SUCCESS)
        printf("GetExternalIPAddress failed.\n");
    else
        printf("ExternalIPAddress = %s\n", externalIPAddress);

    /*if (addAny) {
        r = UPNP_AddAnyPortMapping(urls->controlURL, data->first.servicetype,
                       eport, iport, iaddr, description,
                       proto, 0, leaseDuration, reservedPort);
        if(r==UPNPCOMMAND_SUCCESS)
            eport = reservedPort;
        else
            printf("AddAnyPortMapping(%s, %s, %s) failed with code %d (%s)\n",
                   eport, iport, iaddr, r, strupnperror(r));
    } else {*/
        r = UPNP_AddPortMapping(urls->controlURL, data->first.servicetype,
                    eport, iport, iaddr, description,
                    proto, 0, leaseDuration);
        if(r!=UPNPCOMMAND_SUCCESS)
            printf("AddPortMapping(%s, %s, %s) failed with code %d (%s)\n",
                   eport, iport, iaddr, r, strupnperror(r));
    //}


}

int main(int argc, char ** argv){
	uint16_t port = 6666;
	uint32_t lifetime = 300;
	struct UPNPDev * devlist = 0;
    char lanaddr[64];       /* my ip address on the LAN */
    const char * rootdescurl = 0;
    const char * multicastif = 0;
    const char * minissdpdpath = 0;
    int retcode = 0;
    int error = 0;
    int ipv6 = 0;
	struct UPNPUrls urls;
	struct IGDdatas data;
	char iport[16];
	char eport[16];
//	char * r_iport = 0;
//	char * r_eport = 0;
	const char * proto = "UDP";
	char leaseDuration[32];
	const char * description = "turn server port mapping";
	if( rootdescurl
    || (devlist = upnpDiscover(2000, multicastif, minissdpdpath,
                                     0/*sameport*/, ipv6, &error)))
    {
        struct UPNPDev * device;
        //struct UPNPUrls urls;
        //struct IGDdatas data;
        if(devlist)
        {
            printf("List of UPNP devices found on the network :\n");
            for(device = devlist; device; device = device->pNext)
            {
                printf(" desc: %s\n st: %s\n\n", device->descURL, device->st);
            }
        }
        else if(!rootdescurl)
        {
            printf("upnpDiscover() error code=%d\n", error);
        }
        int i = 1;
        if( (rootdescurl && UPNP_GetIGDFromUrl(rootdescurl, &urls, &data, lanaddr, sizeof(lanaddr)))
        || (i = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr))))
        {
            switch(i) {
            case 1:
                printf("Found valid IGD : %s\n", urls.controlURL);
                break;
            case 2:
                printf("Found a (not connected?) IGD : %s\n", urls.controlURL);
                printf("Trying to continue anyway\n");
                break;
            case 3:
                printf("UPnP device found. Is it an IGD ? : %s\n", urls.controlURL);
                printf("Trying to continue anyway\n");
                break;
            default:
                printf("Found device (igd ?) : %s\n", urls.controlURL);
                printf("Trying to continue anyway\n");
            }
        }
        printf("local LAN address: %s\n", lanaddr);
	}
    sprintf (iport, "%d",port);
	printf("iport: %s\n", iport);
    sprintf (eport, "%d",port);
	printf("eport: %s\n", eport);
    sprintf (leaseDuration, "%d",lifetime);
    SetRedirectAndTest(&urls, &data, lanaddr, iport, eport, proto, leaseDuration, description, 0);
	
	return retcode;
}	
	


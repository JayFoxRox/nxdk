// Implements a TCP Based Echo Service. See https://tools.ietf.org/html/rfc862

#include "lwip/debug.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/timers.h"
#include "netif/etharp.h"
#include "pktdrv.h"
#include <hal/input.h>
#include <hal/xbox.h>
#include <pbkit/pbkit.h>
#include <xboxkrnl/xboxkrnl.h>
#include <xboxrt/debug.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include <string.h>


#ifdef DEBUG
#warning Debugging is enabled
#endif

#ifndef DEBUG
#define LWIP_DBG         LWIP_DBG_OFF
#endif

#define PKT_TMR_INTERVAL 5 /* ms */


struct netif nforce_netif, *g_pnetif;

err_t nforceif_init(struct netif *netif);
static void packet_timer(void *arg);

static void tcpip_init_done(void *arg)
{
	sys_sem_t *init_complete = arg;
	sys_sem_signal(init_complete);
}

static void packet_timer(void *arg)
{
  LWIP_UNUSED_ARG(arg);
  Pktdrv_ReceivePackets();
  sys_timeout(PKT_TMR_INTERVAL, packet_timer, NULL);
}



static void memstats() {
  MM_STATISTICS ms;
  ms.Length = sizeof(MM_STATISTICS);
  MmQueryStatistics(&ms);

	#define PRINT(stat) debugPrint(# stat ": %d   ", ms.stat);
  PRINT(TotalPhysicalPages)
  PRINT(AvailablePages)
  PRINT(VirtualMemoryBytesCommitted)
  PRINT(VirtualMemoryBytesReserved)
  PRINT(CachePagesCommitted)
  PRINT(PoolPagesCommitted)
  PRINT(StackPagesCommitted)
  PRINT(ImagePagesCommitted)
  debugPrint("\n");
}





/** Serve one echo connection accepted in the echo thread */
static void echo_server_netconn_serve(struct netconn *conn)
{
  struct netbuf *inbuf;
  char *buf;
  u16_t buflen;
  err_t err;
  ip_addr_t naddr;
  u16_t port = 0;

  /* Get and display remote ip address and request headers */
  netconn_peer(conn, &naddr, &port);
  debugPrint("[Connection from %s]\n", ip4addr_ntoa(ip_2_ip4(&naddr)));
  
  /* Read the data from the port, blocking if nothing yet there. 
   We assume the request (the part we care about) is in one netbuf */
  while(1) {  

    memstats();

    err = netconn_recv(conn, &inbuf);
  
    if (err == ERR_OK) {

      netbuf_data(inbuf, (void**)&buf, &buflen);

      debugPrint("[Received %d bytes]\n", buflen);

      /* Respond with what we got */
      netconn_write(conn, buf, buflen, NETCONN_COPY);

      /* Delete the buffer (netconn_recv gives us ownership,
      so we have to make sure to deallocate the buffer) */
      netbuf_delete(inbuf);

    } else if (err == ERR_RST) {
      debugPrint("[Connection was reset]\n");
      break;
    } else if (err == ERR_CLSD) {
      debugPrint("[Connection was closed]\n");
      break;
    } else {
      debugPrint("[netconn_recv failed with %d]\n", err);
      break;
    }
    
  }

}

/** The main function, never returns! */
static void echo_server_netconn_work()
{
  struct netconn *conn, *newconn;
  err_t err;
  
  /* Create a new TCP connection handle */
  conn = netconn_new(NETCONN_TCP);
  LWIP_ERROR("echo_server: invalid conn", (conn != NULL), return;);
  
  /* Bind to port 7 (echo) with default IP address */
  netconn_bind(conn, NULL, 7);
  
  /* Put the connection into LISTEN state */
  netconn_listen(conn);
  
  do {
    err = netconn_accept(conn, &newconn);
    if (err == ERR_OK) {

      //FIXME: Create a new thread for this connection?
      echo_server_netconn_serve(newconn);

      netconn_delete(newconn);
    }
  } while(err == ERR_OK);
  LWIP_DEBUGF(LWIP_DBG,
    ("echo_server_netconn_thread: netconn_accept received error %d, shutting down",
    err));
  netconn_close(conn);
  netconn_delete(conn);
}

void main(void)
{
	sys_sem_t init_complete;
	const ip4_addr_t *ip;
	static ip4_addr_t ipaddr, netmask, gw;

#ifdef DEBUG
	asm volatile ("jmp .");
	debug_flags = LWIP_DBG_ON;
#else
	debug_flags = 0;
#endif

#if 0 // Static IP = 0; DHCP = 1
	IP4_ADDR(&gw, 0,0,0,0);
	IP4_ADDR(&ipaddr, 0,0,0,0);
	IP4_ADDR(&netmask, 0,0,0,0);
#else
	IP4_ADDR(&gw, 192,168,177,1);
	IP4_ADDR(&ipaddr, 192,168,177,3);
	IP4_ADDR(&netmask, 255,255,255,0);
#endif

	/* Initialize the TCP/IP stack. Wait for completion. */
	sys_sem_new(&init_complete, 0);
	tcpip_init(tcpip_init_done, &init_complete);
	sys_sem_wait(&init_complete);
	sys_sem_free(&init_complete);

	pb_init();
	pb_show_debug_screen();

	g_pnetif = netif_add(&nforce_netif, &ipaddr, &netmask, &gw,
	                     NULL, nforceif_init, ethernet_input);
	if (!g_pnetif) {
		debugPrint("netif_add failed\n");
		return;
	}

	netif_set_default(g_pnetif);
	netif_set_up(g_pnetif);

#if USE_DHCP
	dhcp_start(g_pnetif);
#endif

	packet_timer(NULL);

#if USE_DHCP
	debugPrint("Waiting for DHCP...\n");
	while (g_pnetif->dhcp->state != DHCP_STATE_BOUND)
		NtYieldExecution();
	debugPrint("DHCP bound!\n");
#endif

	debugPrint("\n");
	debugPrint("IP address.. %s\n", ip4addr_ntoa(netif_ip4_addr(g_pnetif)));
	debugPrint("Mask........ %s\n", ip4addr_ntoa(netif_ip4_netmask(g_pnetif)));
	debugPrint("Gateway..... %s\n", ip4addr_ntoa(netif_ip4_gw(g_pnetif)));
	debugPrint("\n");

  while(1) {
    echo_server_netconn_work();
  }

	Pktdrv_Quit();
	return;
}

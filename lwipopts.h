#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define NO_SYS                          1
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_API                  0
#define LWIP_DHCP                       1
#define LWIP_IPV4                       1
#define LWIP_ICMP                       1
#define LWIP_DNS                        1
#define LWIP_UDP                        1
#define LWIP_TCP                        1
#define TCP_QUEUE_OOSEQ                 0
#define MEM_LIBC_MALLOC                 0
#define MEM_ALIGNMENT                   4
#define MEMP_NUM_TCP_PCB                3
#define MEMP_NUM_UDP_PCB                2
#define LWIP_TIMEVAL_PRIVATE            0


//-----------------------------------------------------------
// Memory and buffer sizes
//-----------------------------------------------------------
#define MEM_SIZE                        (12 * 1024)
#define MEMP_NUM_PBUF                   32
#define MEMP_NUM_TCP_SEG                32
#define MEMP_NUM_SYS_TIMEOUT            20
#define PBUF_POOL_SIZE                  16

//-----------------------------------------------------------
// Enable core features
//-----------------------------------------------------------
#define LWIP_NETIF_API                  0
#define LWIP_SOCKET                     0
#define LWIP_NETCONN                    0
#define LWIP_RAW                        1

//-----------------------------------------------------------
// MQTT support
//-----------------------------------------------------------
#define LWIP_MQTT                       1

//-----------------------------------------------------------
// Misc utilities
//-----------------------------------------------------------
#define TCP_MSS                         1460
#define TCP_SND_BUF                     (2 * TCP_MSS)
#define TCP_WND                         (2 * TCP_MSS)
#define LWIP_STATS                      0
#define LWIP_PROVIDE_ERRNO              1
#endif /* __LWIPOPTS_H__ */

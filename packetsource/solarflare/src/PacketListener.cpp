#include "PacketListener.h"

#include "vi.h"
#include "pd.h"
#include "memreg.h"
#include "packedstream.h"
#include "utils.h"
#include <locale.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip.h>

void* InterfaceMonitorThread(void* arg)
{
	InterfaceMonitor *ft = (InterfaceMonitor*)arg;
	ft->run();
	return NULL;
}

PacketListener::PacketListener(int perListenerRouters, int index, int intfId) {
	this->_name = "PacketListener";
	this->setLogLevel(Log::theLog().level());
	ROUTER_TO_PROCESS = 0;
	END_ROUTER_ID = perListenerRouters;
	InterfaceName = IPGlobal::SOLAR_INTERFACES[index];
	InterfaceId = intfId;
	cfg_hexdump = 0;
	cfg_timestamping = IPGlobal::SOLARFLARE_HW_TIMESTAMP;
	cfg_verbose = 0;
	cfg_max_fill = 0;
	pkt = NULL;
	len = 0;
	tv_sec = 0;
	tv_nsec = 0;
	t_index = 0;
	maxPktLen = IPGlobal::MAX_PKT_LEN_PER_INTERFACE[intfId];
}

PacketListener::~PacketListener()
{}

VOID PacketListener::hexdump(const void* pv, int len)
{
  const unsigned char* p = (const unsigned char*) pv;
  int i;
  for( i = 0; i < len; ++i ) {
    const char* eos;
    switch( i & 15 ) {
    case 0:
      printf("%08x  ", i);
      eos = "";
      break;
    case 1:
      eos = " ";
      break;
    case 15:
      eos = "\n";
      break;
    default:
      eos = (i & 1) ? " " : "";
      break;
    }
    printf("%02x%s", (unsigned) p[i], eos);
  }
  printf(((len & 15) == 0) ? "\n" : "\n\n");
}

BOOL PacketListener::isRepositoryInitialized() {
	return repoInitStatus;
}

static inline void posted_buf_put(interfaceThread* t, struct buf* buf)
{
  buf->next = NULL;
  *(t->posted_bufs_tail) = buf;
  t->posted_bufs_tail = &buf->next;
}

static inline struct buf* posted_buf_get(interfaceThread* t)
{
  struct buf* buf = t->posted_bufs;
  if( buf != NULL ) {
    t->posted_bufs = buf->next;
    if( t->posted_bufs == NULL )
      t->posted_bufs_tail = &(t->posted_bufs);
  }
  return buf;
}

VOID PacketListener::consume_packet(ef_packed_stream_packet* ps_pkt)
{
	pkt = (BYTE)ef_packed_stream_packet_payload(ps_pkt);

	uint16_t protocol = pkt[12] * 256 + pkt[13];		/* Ethernet Containing Protocol */

	switch(protocol)
	{
		case ETH_IP:			/* Internet Protocol packet	*/
		case ETH_8021Q:			/* 802.1Q VLAN Extended Header  */
		case ETH_MPLS_UC:		/* MPLS */
						break;
		default:
						countDiscardedPkt();
						return;
						break;
	}

	len = ps_pkt->ps_cap_len;

    timeval curTime;
    gettimeofday(&curTime, NULL);

	if(cfg_timestamping)
	{
		tv_sec = ps_pkt->ps_ts_sec;
		tv_nsec = ps_pkt->ps_ts_nsec;
		t_index = PKT_WRITE_TIME_INDEX(ps_pkt->ps_ts_sec,IPGlobal::TIME_INDEX);
	}
	else
	{
//		tv_sec = curTime.tv_sec;
//		tv_nsec = ((curTime.tv_sec * 1000000 + curTime.tv_usec) * 1000);

		tv_sec = IPGlobal::CURRENT_EPOCH_SEC;
		tv_nsec = IPGlobal::CURRENT_EPOCH_NANO_SEC;

		t_index = PKT_WRITE_TIME_INDEX(tv_sec,IPGlobal::TIME_INDEX);
	}

	if(len >= maxPktLen) copy_len = maxPktLen;
	else copy_len = len;

	if(!IPGlobal::PACKET_PROCESSING) return; /* If packet processing is false don't Push Packet */

	if(PKTStore::pktRepository_busy_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index] || PKTStore::pktRepository_cnt_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index] >= MAX_PKT_ALLOWED_PER_TIME_INDEX) return;
	RawPkt *rpkt = PKTStore::pktRepository_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index][PKTStore::pktRepository_cnt_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index]];
	rpkt->len = len;
	rpkt->tv_sec = tv_sec;
	rpkt->tv_nsec = tv_nsec;
	memcpy((void *)rpkt->pkt, (const void *)pkt, copy_len);
	PKTStore::pktRepository_cnt_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index]++;

	ROUTER_TO_PROCESS++;
	if(ROUTER_TO_PROCESS >= END_ROUTER_ID)
		ROUTER_TO_PROCESS = 0;
}

VOID PacketListener::handle_rx_ps(interfaceThread* t, const ef_event* pev)
{
  int n_pkts, n_bytes, rc;

  if( EF_EVENT_RX_PS_NEXT_BUFFER(*pev) ) {
    if( t->current_buf != NULL ) {
      TRY(ef_vi_receive_post(&t->vi, t->current_buf->ef_address, 0));
      posted_buf_put(t, t->current_buf);
    }
    t->current_buf = posted_buf_get(t);
    t->ps_pkt_iter = ef_packed_stream_packet_first(t->current_buf, t->psp_start_offset);
  }

  //Date change check to reset the counter
  if(IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[InterfaceId])
  {
	  TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), t->n_rx_pkts);
	  t->n_rx_pkts = 0;
	  t->n_rx_bytes = 0;
	  IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[InterfaceId] = false;
  }

  ef_packed_stream_packet* ps_pkt = t->ps_pkt_iter;
  rc = ef_vi_packed_stream_unbundle(&t->vi, pev, &t->ps_pkt_iter, &n_pkts, &n_bytes);

  t->n_rx_pkts += n_pkts;
  t->n_rx_bytes += n_bytes;

  if( cfg_verbose )
    printf("EVT: rc=%d n_pkts=%d n_bytes=%d\n", rc, n_pkts, n_bytes);

  int i;
  for( i = 0; i < n_pkts; ++i ) {
    consume_packet(ps_pkt);
    ps_pkt = ef_packed_stream_packet_next(ps_pkt);
  }
}

VOID PacketListener::receivePackets(interfaceThread* t)
{
	  ef_event evs[64];//[16];
	  const int max_evs = sizeof(evs) / sizeof(evs[0]);
	  int i, n_ev;

	  while(IPGlobal::PKT_LISTENER_RUNNING_STATUS[InterfaceId]) {
		n_ev = ef_eventq_poll(&t->vi, evs, max_evs);

		for( i = 0; i < n_ev; ++i ) {
		  switch( EF_EVENT_TYPE(evs[i]) ) {
		  case EF_EVENT_TYPE_RX_PACKED_STREAM:
			handle_rx_ps(t, &(evs[i]));
			break;
		  default:
			LOGE("ERROR: unexpected event type=%d\n", (int) EF_EVENT_TYPE(evs[i]));
			break;
		  }
		}
	  }
	  IPGlobal::PKT_LISTENER_INTF_MON_RUNNING_STATUS[InterfaceId] = false;
	  printf("SOLAR [%10s] Shutdown Completed.\n", InterfaceName.c_str()); // InterfaceName.c_str()
}

VOID PacketListener::start()
{
	  pthread_t imThread;
	  interfaceThread* t;
	  unsigned vi_flags;
	  int c, i;

//	  resetPktCounter();

	  MAX_PKT_ALLOWED_PER_TIME_INDEX = (int)(((IPGlobal::PPS_PER_INTERFACE[InterfaceId] / IPGlobal::ROUTER_PER_INTERFACE[InterfaceId]) /100 ) * IPGlobal::PPS_CAP_PERCENTAGE[InterfaceId]);

	  printf("Solarflare started with [%d] Routers for Interface [%d]->[%s] with %d% [%d] pps cap\n", END_ROUTER_ID, InterfaceId, InterfaceName.c_str(), IPGlobal::PPS_CAP_PERCENTAGE[InterfaceId], MAX_PKT_ALLOWED_PER_TIME_INDEX);
	  TheLog_nc_v5(Log::Info, name(),"  Solarflare started with [%d] Routers for Interface [%d]->[%s] with %d% [%d] pps cap\n", END_ROUTER_ID, InterfaceId, InterfaceName.c_str(), IPGlobal::PPS_CAP_PERCENTAGE[InterfaceId], MAX_PKT_ALLOWED_PER_TIME_INDEX);

	  TEST((t = (interfaceThread*)calloc(1, sizeof(*t))) != NULL);
	  t->current_buf = NULL;
	  t->posted_bufs = NULL;
	  t->posted_bufs_tail = &(t->posted_bufs);


	  TRY(ef_driver_open(&t->dh));
	  TRY(ef_pd_alloc_by_name(&t->pd, t->dh, InterfaceName.c_str(), EF_PD_RX_PACKED_STREAM));

	  vi_flags = EF_VI_RX_PACKED_STREAM | EF_VI_RX_PS_BUF_SIZE_64K;

	  if( cfg_timestamping )
		vi_flags |= EF_VI_RX_TIMESTAMPS;
	  TRY(ef_vi_alloc_from_pd(&t->vi, t->dh, &t->pd, t->dh, -1, -1, -1, NULL, -1, (enum ef_vi_flags)vi_flags));

	  ef_packed_stream_params psp;
	  TRY(ef_vi_packed_stream_get_params(&t->vi, &psp));

	  if( cfg_max_fill == 0 )
	      cfg_max_fill = psp.psp_max_usable_buffers;

	  fprintf(stderr, "\nREQ_SIZE              = %d\n", ef_vi_receive_capacity(&t->vi));
//	  fprintf(stderr, "EVQ_SIZE              = %d\n", ef_eventq_capacity(&t->vi));
	  fprintf(stderr, "PSP_BUFFER_SIZE       = %d\n", psp.psp_buffer_size);
	  fprintf(stderr, "PSP_BUFFER_ALIGN      = %d\n", psp.psp_buffer_align);
	  fprintf(stderr, "PSP_START_OFFSET      = %d\n", psp.psp_start_offset);
	  fprintf(stderr, "PSP_MAX_USABLE_BUFFER = %d\n\n\n", psp.psp_max_usable_buffers);
	  t->psp_start_offset = psp.psp_start_offset;

	  TEST( cfg_max_fill <= ef_vi_receive_capacity(&t->vi) );

	  /* Packed stream mode requires large contiguous buffers, so allocate huge
	   * pages.  (Also makes consuming packets more efficient of course).
	   */
	  int n_bufs = cfg_max_fill;
	  size_t buf_size = psp.psp_buffer_size;
	  size_t alloc_size = n_bufs * buf_size;
	  alloc_size = ROUND_UP(alloc_size, huge_page_size);

	  void* p;
	  p = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

	  if( p == MAP_FAILED ) {
	    fprintf(stderr, "ERROR: mmap failed.  You probably need to allocate some huge pages....\n");
	    exit(2);
	  }

	  TEST(p != MAP_FAILED);
	  TEST(((uintptr_t) p & (psp.psp_buffer_align - 1)) == 0);
	  TRY(ef_memreg_alloc(&t->memreg, t->dh, &t->pd, t->dh, p, alloc_size));

	  for( i = 0; i < n_bufs; ++i ) {
		struct buf* buf = (struct buf*) ((char*) p + i * buf_size);
		buf->ef_address = ef_memreg_dma_addr(&t->memreg, i * buf_size);
		TRY(ef_vi_receive_post(&t->vi, buf->ef_address, 0));
		posted_buf_put(t, buf);
	  }

	  ef_filter_spec filter_spec;
	  ef_filter_spec_init(&filter_spec, EF_FILTER_FLAG_NONE);
	  TRY(ef_filter_spec_set_unicast_all(&filter_spec));
	  TRY(ef_vi_filter_add(&t->vi, t->dh, &filter_spec, NULL));

	//  TEST(pthread_create(&thread_id, NULL, monitor_fn, t) == 0);
	  IPGlobal::PKT_LISTENER_INTF_MON_RUNNING_STATUS[InterfaceId] = true;
	  InterfaceMonitor *im = new InterfaceMonitor(InterfaceId, t);
	  pthread_create(&imThread, NULL, InterfaceMonitorThread, im);

	  repoInitStatus = true;

	  receivePackets(t);

	  pthread_join(imThread,0);
}

VOID PacketListener::countDiscardedPkt(){

	IPGlobal::ip_discarded_large_packets_i[InterfaceId]++;
}

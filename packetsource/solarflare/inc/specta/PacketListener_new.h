#ifndef SRC_PACKETLISTENER_H_
#define SRC_PACKETLISTENER_H_

#include <string>

#include "SolarGlobal.h"
#include "Log.h"
#include "SpectaTypedef.h"
#include "InterfaceMonitor.h"
#include "BaseConfig.h"

#define	ETHERNET_HDR_LEN	14
#define	IPV4_HDR_LEN		20
#define BYTE_TO_COPY		48

using namespace std;

class PacketListener : public BaseConfig {

	public:
		PacketListener(int perListenerRouters, int index, int intfId);
		~PacketListener();
		VOID start();
		BOOL  	isRepositoryInitialized();

	private:
		int 	ROUTER_TO_PROCESS 	= 0;
		int 	END_ROUTER_ID 		= 0;
		string 	InterfaceName;
		int 	InterfaceId 		= 0;
		uint64_t PKT_COUNTER 	= 0;
		int 	MAX_PKT_ALLOWED_PER_TIME_INDEX = 0;

		int cfg_hexdump;
		int cfg_timestamping;
		int cfg_max_fill;
		int cfg_verbose;

		BYTE 		pkt;
		uint16_t 	len;
		uint32_t 	tv_sec;
		uint128_t 	tv_nsec;

		uint16_t 	maxPktLen;
		uint16_t 	copy_len = 0;

		int t_index;
		BOOL repoInitStatus = false;

		static void* thread_fn(void* arg);

		int init_vi_set(const char* intf, int n_threads);
		int init_vi(struct vi_state* vi_state);
		void loop(struct vi_state* vi_state);
		void handle_rx(struct vi_state* vi_state, int pkt_buf_i, int len);
		void vi_refill_rx_ring(struct vi_state* vi_state);
		void pkt_buf_free(struct vi_state* vi_state, struct pkt_buf* pkt_buf);
		VOID hexdump(const void* pv, int len);
		struct pkt_buf* pkt_buf_from_id(struct vi_state* vi_state, int pkt_buf_i);

		VOID consume_packet(const void* pv, int len);
		VOID handle_rx_ps(interfaceThread* t, const ef_event* pev);

		VOID setPktCounter();
		VOID resetPktCounter();

		VOID countDiscardedPkt();

		VOID storepkt_i_0();
		VOID storepkt_i_1();
		VOID storepkt_i_2();
		VOID storepkt_i_3();
		VOID storepkt_i_4();
		VOID storepkt_i_5();
		VOID storepkt_i_6();
		VOID storepkt_i_7();

		VOID storepkt_i_0_r_0();
		VOID storepkt_i_0_r_1();
		VOID storepkt_i_0_r_2();
		VOID storepkt_i_0_r_3();
		VOID storepkt_i_0_r_4();
		VOID storepkt_i_0_r_5();
		VOID storepkt_i_0_r_6();
		VOID storepkt_i_0_r_7();
		VOID storepkt_i_0_r_8();
		VOID storepkt_i_0_r_9();
		VOID storepkt_i_0_r_10();
		VOID storepkt_i_0_r_11();

		VOID storepkt_i_1_r_0();
		VOID storepkt_i_1_r_1();
		VOID storepkt_i_1_r_2();
		VOID storepkt_i_1_r_3();
		VOID storepkt_i_1_r_4();
		VOID storepkt_i_1_r_5();
		VOID storepkt_i_1_r_6();
		VOID storepkt_i_1_r_7();
		VOID storepkt_i_1_r_8();
		VOID storepkt_i_1_r_9();
		VOID storepkt_i_1_r_10();
		VOID storepkt_i_1_r_11();

		VOID storepkt_i_2_r_0();
		VOID storepkt_i_2_r_1();
		VOID storepkt_i_2_r_2();
		VOID storepkt_i_2_r_3();
		VOID storepkt_i_2_r_4();
		VOID storepkt_i_2_r_5();
		VOID storepkt_i_2_r_6();
		VOID storepkt_i_2_r_7();
		VOID storepkt_i_2_r_8();
		VOID storepkt_i_2_r_9();
		VOID storepkt_i_2_r_10();
		VOID storepkt_i_2_r_11();

		VOID storepkt_i_3_r_0();
		VOID storepkt_i_3_r_1();
		VOID storepkt_i_3_r_2();
		VOID storepkt_i_3_r_3();
		VOID storepkt_i_3_r_4();
		VOID storepkt_i_3_r_5();
		VOID storepkt_i_3_r_6();
		VOID storepkt_i_3_r_7();
		VOID storepkt_i_3_r_8();
		VOID storepkt_i_3_r_9();
		VOID storepkt_i_3_r_10();
		VOID storepkt_i_3_r_11();

		VOID storepkt_i_4_r_0();
		VOID storepkt_i_4_r_1();
		VOID storepkt_i_4_r_2();
		VOID storepkt_i_4_r_3();
		VOID storepkt_i_4_r_4();
		VOID storepkt_i_4_r_5();
		VOID storepkt_i_4_r_6();
		VOID storepkt_i_4_r_7();
		VOID storepkt_i_4_r_8();
		VOID storepkt_i_4_r_9();
		VOID storepkt_i_4_r_10();
		VOID storepkt_i_4_r_11();

		VOID storepkt_i_5_r_0();
		VOID storepkt_i_5_r_1();
		VOID storepkt_i_5_r_2();
		VOID storepkt_i_5_r_3();
		VOID storepkt_i_5_r_4();
		VOID storepkt_i_5_r_5();
		VOID storepkt_i_5_r_6();
		VOID storepkt_i_5_r_7();
		VOID storepkt_i_5_r_8();
		VOID storepkt_i_5_r_9();
		VOID storepkt_i_5_r_10();
		VOID storepkt_i_5_r_11();

		VOID storepkt_i_6_r_0();
		VOID storepkt_i_6_r_1();
		VOID storepkt_i_6_r_2();
		VOID storepkt_i_6_r_3();
		VOID storepkt_i_6_r_4();
		VOID storepkt_i_6_r_5();
		VOID storepkt_i_6_r_6();
		VOID storepkt_i_6_r_7();
		VOID storepkt_i_6_r_8();
		VOID storepkt_i_6_r_9();
		VOID storepkt_i_6_r_10();
		VOID storepkt_i_6_r_11();

		VOID storepkt_i_7_r_0();
		VOID storepkt_i_7_r_1();
		VOID storepkt_i_7_r_2();
		VOID storepkt_i_7_r_3();
		VOID storepkt_i_7_r_4();
		VOID storepkt_i_7_r_5();
		VOID storepkt_i_7_r_6();
		VOID storepkt_i_7_r_7();
		VOID storepkt_i_7_r_8();
		VOID storepkt_i_7_r_9();
		VOID storepkt_i_7_r_10();
		VOID storepkt_i_7_r_11();


};

#endif /* SRC_PACKETLISTENER_H_ */

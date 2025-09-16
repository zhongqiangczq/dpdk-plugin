#pragma once

#include <inttypes.h>
#include <net/if.h>
#include <stdint.h>
#include <ctime>
#include <string>

// DPDK headers - use conditional compilation for IDE compatibility
#ifdef __has_include
#if __has_include(<rte_eal.h>)
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_latencystats.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_version.h>
#include <rte_mempool.h>
#include <rte_ring.h>
#define DPDK_HEADERS_AVAILABLE 1
#else
#define DPDK_HEADERS_AVAILABLE 0
#endif
#else
// Fallback for compilers without __has_include
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_latencystats.h>
#include <rte_lcore.h>
#include <rte_mbuf.h>
#include <rte_version.h>
#include <rte_mempool.h>
#include <rte_ring.h>
#define DPDK_HEADERS_AVAILABLE 1
#endif

// Define VLAN header structure manually since rte_vlan.h is not available in DPDK 23.11.4
struct rte_vlan_hdr {
    uint16_t vlan_tci;  /**< Priority (3) + CFI (1) + Identifier Code (12) */
    uint16_t eth_proto; /**< Ethernet type of encapsulated frame. */
} __attribute__((__packed__));

// Zeek headers - use conditional compilation for IDE compatibility
#ifdef __has_include
#if __has_include(<zeek/iosource/PktSrc.h>)
#include <zeek/iosource/PktSrc.h>
#include <zeek/Packet.h>
#include <zeek/Dict.h>
#include <zeek/Val.h>
#include <zeek/ID.h>
#include <zeek/Reporter.h>
#include <zeek/RunState.h>
#include <zeek/util.h>
#define ZEEK_HEADERS_AVAILABLE 1
#else
// Forward declarations and stub definitions for IDE compatibility when Zeek headers are not found
namespace zeek {
    namespace iosource {
        class PktSrc {
        public:
            struct Stats {
                uint64_t received;
                uint64_t dropped;
                uint64_t link;
            };
            virtual ~PktSrc() = default;
            virtual void Open() {}
            virtual void Close() {}
            virtual bool ExtractNextPacket(void*) { return false; }
            virtual void DoneWithPacket() {}
            virtual bool PrecompileFilter(int, const std::string&) { return false; }
            virtual bool SetFilter(int) { return false; }
            virtual void Statistics(Stats*) {}
            virtual void Process() {}
            virtual double GetNextTimeout() { return 0.0; }
        };
    }
    class Packet {};
     struct Properties {
         const char* path = nullptr;
         const char* interface = nullptr;
     };
}
#define ZEEK_HEADERS_AVAILABLE 0
#endif
#else
// Fallback for compilers without __has_include
#include <zeek/iosource/PktSrc.h>
#include <zeek/Packet.h>
#include <zeek/Dict.h>
#include <zeek/Val.h>
#include <zeek/ID.h>
#include <zeek/Reporter.h>
#include <zeek/RunState.h>
#include <zeek/util.h>
#define ZEEK_HEADERS_AVAILABLE 1
#endif

// Should be 2**n - 1
//#define NUM_MBUFS 32767
#define NUM_MBUFS 32767
// NUM_BUFS % MBUF_CACHE_SIZE should be 0
#define MBUF_CACHE_SIZE RTE_MEMPOOL_CACHE_MAX_SIZE

#define CACHE_SIZE 1024*1024*1024
#define BURST_SIZE 2048
#define RX_RING_SIZE 4096

/*
 * The overhead from max frame size to MTU.
 * We have to consider the max possible overhead.
 */
#define MTU_OVERHEAD (RTE_ETHER_HDR_LEN + RTE_ETHER_CRC_LEN + 2 * sizeof(struct rte_vlan_hdr))

/* allow max jumbo frame 9726 */
#define JUMBO_FRAME_MAX_SIZE 0x2600



namespace zeek::iosource
	{

struct worker_thread_args {
    int port_id;
	int queue_id;
    struct rte_ring *ring;
};
static uint64_t queue_drops;

static int GrabPackets(void *args_ptr);
static bool pkt_loop;

class DPDK : public PktSrc
	{
public:
	/**
	 * Constructor.
	 *
	 * path: Name of the interface to open
	 *
	 * is_live: Must be true
	 */
	DPDK(const std::string& path, bool is_live);

	/**
	 * Destructor.
	 */
#if ZEEK_HEADERS_AVAILABLE
	virtual ~DPDK();
#else
	~DPDK();
#endif

	static PktSrc* PortInit(const std::string& iface_name, bool is_live);

protected:
	// PktSrc interface.
#if ZEEK_HEADERS_AVAILABLE
	void Open() override;
	void Close() override;

	void Process() override;

	void Statistics(PktSrc::Stats* stats) override;

	void DoneWithPacket() override {};
	bool ExtractNextPacket(zeek::Packet* pkt) override { return true; };
	bool PrecompileFilter(int index, const std::string& filter) override { return true; };
	bool SetFilter(int index) override { return true; };
	double GetNextTimeout() override { return 0; };
#else
	void Open();
	void Close();

	void Process();

	void Statistics(PktSrc::Stats* stats);

	void DoneWithPacket() {};
	bool ExtractNextPacket(zeek::Packet* pkt) { return true; };
	bool PrecompileFilter(int index, const std::string& filter) { return true; };
	bool SetFilter(int index) { return true; };
	double GetNextTimeout() { return 0; };
#endif

private:
	inline int port_init(uint16_t port);

	zeek::Packet* pkt;
	PktSrc::Stats queue_stats;

	uint64_t missed_pkts;

	uint16_t my_port_num;
	uint16_t total_queues;
	uint16_t my_queue_num;

	uint32_t retry_us;

	double no_pkts;
	double full_pkts;
	double partial_pkts;
	double pkt_histo[BURST_SIZE];

	bool debug;

	Properties props;

	// DPDK-related things
	struct rte_mempool* mbuf_pool;
	struct rte_ring* recv_ring;
	};

	} // namespace zeek::iosource

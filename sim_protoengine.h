#ifndef SIM_PROTO_ENGINE_
#define SIM_PROTO_ENGINE_

#include <map>

#include "configuration.h"
#include "sim_router.h"

// changed at 2024-6-4
struct ProtoPacket {
    time_type srcTime, dstTime;
    add_type sourceAddress, destinationAddress;
    long packetSize;
    long protoDesc;
    // 用于数据包的唯一标识
    typedef size_t TId;
    TId id;

    ProtoPacket() {
        size_t addSize = configuration::ap().cube_number();
        sourceAddress.reserve(addSize);
        destinationAddress.reserve(addSize);
    }
};

class ProtoStateMachine {
   public:
    enum Status { IDLE, PRE_SYNC, DATA_TRANS, ACK_TRANS, POST_SYNC, DONE };

   public:
    time_type srcTime, dstTime;
    add_type sourceAddress, destinationAddress;
    long packetSize;
    long protoDesc;
    // 用于数据包的唯一标识
    typedef size_t TId;
    TId id;
    Status status;
    std::vector<time_type> packetDelay;

   public:
    ProtoStateMachine(const ProtoPacket& trans)
        : srcTime(trans.srcTime),
          dstTime(trans.dstTime),
          sourceAddress(trans.sourceAddress),
          destinationAddress(trans.destinationAddress),
          packetSize(trans.packetSize),
          protoDesc(trans.protoDesc),
          id(trans.id),
          status(IDLE) {}
};

// changed at 2024-6-4
class ProtoEngine {
   private:
    vector<ProtoStateMachine> trans_list_;
    map<int, int> barrier_count_map_;
    map<int, vector<int> > barrier_items_map_;

   public:
    ProtoEngine() {}

    SPacket add_trans(const ProtoPacket& trans);

    void update_trans(time_type a, const flit_template& flit);

    void update_trans_normal(time_type a, ProtoStateMachine& trans);
    void update_trans_locker(time_type a, ProtoStateMachine& trans);
    void update_trans_barrier(time_type a, ProtoStateMachine& trans);
};

#endif

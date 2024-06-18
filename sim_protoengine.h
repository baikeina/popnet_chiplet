#ifndef SIM_PROTO_ENGINE_
#define SIM_PROTO_ENGINE_

#include <map>
#include <set>

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

   public:
    ProtoEngine() {}

    SPacket add_trans(const ProtoPacket& trans);

    vector<ProtoStateMachine>::iterator get_trans(flit_template::TPacketId id) {
        for (vector<ProtoStateMachine>::iterator it = trans_list_.begin(); it != trans_list_.end();
             it++) {
            if (it->id == id) {
                return it;
            }
        }
        return trans_list_.end();
    }

    void update_trans(time_type a, const flit_template& flit);

    void update_trans_ack_at_receive_time(time_type a, ProtoStateMachine& trans);
    void update_trans_normal(time_type a, ProtoStateMachine& trans);
};

#endif

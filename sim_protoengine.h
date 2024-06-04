#ifndef SIM_PROTO_ENGINE_
#define SIM_PROTO_ENGINE_

#include "configuration.h"
#include "sim_router.h"

//changed at 2024-6-4
struct ProtoPacket
{
	time_type srcTime,dstTime;
	add_type sourceAddress,destinationAddress;
	long packetSize;
	long protoDesc;
	//用于数据包的唯一标识
	typedef size_t TId;
	TId id;

	ProtoPacket()
	{
		size_t addSize=configuration::ap().cube_number();
		sourceAddress.reserve(addSize);
		destinationAddress.reserve(addSize);
	}
};

class ProtoStateMachine
{
    public:
        enum Status{
            IDLE, PRE_SYNC, DATA_TRANS, ACK_TRANS, POST_SYNC, DONE
        };

    public:
        time_type srcTime,dstTime;
        time_type srcEndTime,dstEndTime;
        add_type sourceAddress,destinationAddress;
        long packetSize;
        long protoDesc;
        //用于数据包的唯一标识
        typedef size_t TId;
        TId id;
        Status status;

    public:
        ProtoStateMachine(const ProtoPacket& trans)
            : srcTime(trans.srcTime), dstTime(trans.dstTime),
              srcEndTime(trans.srcTime), dstEndTime(trans.dstTime),
              sourceAddress(trans.sourceAddress), destinationAddress(trans.destinationAddress),
              packetSize(trans.packetSize), protoDesc(trans.protoDesc), id(trans.id),
              status(IDLE)
        {}
};

//changed at 2024-6-4
class ProtoEngine
{
    private:
        vector<ProtoStateMachine> trans_list_;

    public:
        ProtoEngine() {}

        SPacket add_trans(const ProtoPacket& trans);

        void update_trans(time_type a, const flit_template& flit);
};

#endif

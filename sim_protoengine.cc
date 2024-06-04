
#include "sim_protoengine.h"


SPacket ProtoEngine::add_trans(const ProtoPacket& trans)
{
    ProtoStateMachine sm(trans);
    trans_list_.push_back(sm);

    sm.status = ProtoStateMachine::DATA_TRANS;

    // TODO
    // Generate first packet.
    SPacket packet;
    packet.startTime = trans.srcTime;
    packet.sourceAddress = trans.sourceAddress;
    packet.destinationAddress = trans.destinationAddress;
    packet.packetSize = trans.packetSize;
    packet.id = trans.id;
    return packet;
}

void ProtoEngine::update_trans(time_type a, const flit_template& flit)
{
    for (auto& trans: trans_list_)
    {
        if (trans.id == flit.getPacketId())
        {
            trans.srcEndTime = flit.send_finish_time();
            if (trans.dstEndTime < a)
            {
                trans.dstEndTime = a;
            }
            trans.status = ProtoStateMachine::DONE;

            if (trans.status == ProtoStateMachine::DONE)
            {
                static ofstream ofs(configuration::wap().delayinfo_fname().c_str());
                ofs << (long)trans.srcTime << ' '
                    /* <<a<<' ' */;
                for (auto &x : trans.sourceAddress)
                    ofs << x << ' ';
                for (auto &x : trans.destinationAddress)
                    ofs << x << ' ';
                ofs << (long)(trans.srcEndTime - trans.srcTime) << ' ';
                ofs << (long)(trans.dstEndTime - trans.dstTime) << endl;
            }
        }
    }
}

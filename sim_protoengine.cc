
#include "sim_protoengine.h"
#include "sim_foundation.h"


SPacket ProtoEngine::add_trans(const ProtoPacket& trans)
{
    ProtoStateMachine sm(trans);
    sm.status = ProtoStateMachine::DATA_TRANS;

    // TODO
    // Generate first packet.
    SPacket packet;
    packet.startTime = trans.srcTime;
    packet.sourceAddress = trans.sourceAddress;
    packet.destinationAddress = trans.destinationAddress;
    packet.packetSize = trans.packetSize;
    packet.id = trans.id;

    trans_list_.push_back(sm);

    return packet;
}

void ProtoEngine::update_trans(time_type a, const flit_template& flit)
{
    for (auto& trans: trans_list_)
    {
        if (trans.id == flit.getPacketId())
        {
            trans.packetDelay.push_back(flit.send_finish_time() - flit.start_time());
            trans.packetDelay.push_back(a - flit.start_time());

            if (trans.protoDesc & 0x10) { // Locker
                std::cout << "Locker" << std::endl;
                if (trans.status == ProtoStateMachine::DATA_TRANS)
                {
                    // Add new packet.
                    SPacket packet;
                    packet.startTime = (a > trans.dstTime) ? a : trans.dstTime;
                    packet.sourceAddress = trans.destinationAddress;
                    packet.destinationAddress = trans.sourceAddress;
                    packet.packetSize = 1;
                    packet.id = trans.id;
                    std::cout << "update_trans " << packet.startTime << std::endl;

                    sim_foundation::wsf().inputTrace(packet);
                    sim_foundation::wsf().router(packet.sourceAddress).inputTrace(packet);

                    trans.status = ProtoStateMachine::ACK_TRANS;
                } else {
                    trans.status = ProtoStateMachine::DONE;
                }
            } else {
                trans.status = ProtoStateMachine::DONE;
            }

            if (trans.status == ProtoStateMachine::DONE)
            {
                static ofstream ofs(configuration::wap().delayinfo_fname().c_str());
                ofs << (long)trans.srcTime << ' '
                    /* <<a<<' ' */;
                for (auto &x : trans.sourceAddress)
                    ofs << x << ' ';
                for (auto &x : trans.destinationAddress)
                    ofs << x << ' ';
                ofs << trans.protoDesc << ' ' << trans.packetDelay.size() << ' ';
                for (auto &x : trans.packetDelay)
                    ofs << (long)x << ' ';
                ofs << endl;
            }
        }
    }
}

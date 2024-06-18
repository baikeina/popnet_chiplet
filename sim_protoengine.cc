
#include "sim_protoengine.h"

#include "mess_queue.h"
#include "sim_foundation.h"

#define SPD_LAUNCH 0x10000
#define SPD_BARRIER 0x20000
#define SPD_LOCK 0x40000
#define SPD_UNLOCK 0x80000
#define SPD_BARRIER_COUNT_MASK 0x0FFFF

SPacket ProtoEngine::add_trans(const ProtoPacket& trans) {
    ProtoStateMachine sm(trans);
    sm.status = ProtoStateMachine::DATA_TRANS;

    // Generate first packet.
    SPacket packet;
    packet.startTime = trans.srcTime;
    packet.sourceAddress = trans.sourceAddress;
    packet.destinationAddress = trans.destinationAddress;
    if (sm.protoDesc & (SPD_BARRIER | SPD_LOCK | SPD_UNLOCK)) {
        for (int i = 0; i < packet.destinationAddress.size(); i++) {
            packet.destinationAddress[i] = 0;
        }
    }
    packet.packetSize = trans.packetSize;
    packet.id = trans.id;

    trans_list_.push_back(sm);

    return packet;
}

void ProtoEngine::update_trans(time_type a, const flit_template& flit) {
    std::vector<ProtoStateMachine>::iterator trans_it = get_trans(flit.getPacketId());

    trans_it->packetDelay.push_back(flit.send_finish_time() - flit.start_time());
    trans_it->packetDelay.push_back(a - flit.start_time());

    if (trans_it->protoDesc & (SPD_LAUNCH | SPD_BARRIER | SPD_LOCK | SPD_UNLOCK)) { // Launch
        update_trans_ack_at_receive_time(a, *trans_it);
    } else {
        update_trans_normal(a, *trans_it);
    }

    if (trans_it->status == ProtoStateMachine::DONE) {
        static ofstream ofs(configuration::wap().delayinfo_fname().c_str());
        ofs << (long)trans_it->srcTime << ' '
            /* <<a<<' ' */;
        for (auto& x : trans_it->sourceAddress) ofs << x << ' ';
        for (auto& x : trans_it->destinationAddress) ofs << x << ' ';
        ofs << trans_it->protoDesc << ' ' << trans_it->packetDelay.size() << ' ';
        for (auto& x : trans_it->packetDelay) ofs << (long)x << ' ';
        ofs << endl;
    }
}

void ProtoEngine::update_trans_normal(time_type a, ProtoStateMachine& trans) {
    trans.status = ProtoStateMachine::DONE;
}

void ProtoEngine::update_trans_ack_at_receive_time(time_type a, ProtoStateMachine& trans) {
    if (trans.status == ProtoStateMachine::DATA_TRANS) {
        // Add new packet.
        SPacket packet;
        packet.startTime = (a > trans.dstTime) ? a : trans.dstTime;
        packet.sourceAddress = trans.destinationAddress;
        if (trans.protoDesc & (SPD_BARRIER | SPD_LOCK | SPD_UNLOCK)) {
            for (int i = 0; i < packet.sourceAddress.size(); i++) {
                packet.sourceAddress[i] = 0;
            }
        }
        packet.destinationAddress = trans.sourceAddress;
        packet.packetSize = 1;
        packet.id = trans.id;
        std::cout << "ack_to_trans " << packet.startTime << std::endl;

        sim_foundation::wsf().inputTrace(packet);
        sim_foundation::wsf().router(packet.sourceAddress).inputTrace(packet);
        mess_queue::wm_pointer().update_EVG_cycle(a);

        trans.status = ProtoStateMachine::ACK_TRANS;
    } else {
        trans.status = ProtoStateMachine::DONE;
    }
}


#include "sim_protoengine.h"

#include "sim_foundation.h"
#include "mess_queue.h"

#define SPD_LOCKER 0x10000
#define SPD_BARRIER 0x20000
#define SPD_BARRIER_COUNT_MASK 0x0FFFF

SPacket ProtoEngine::add_trans(const ProtoPacket& trans) {
    ProtoStateMachine sm(trans);
    sm.status = ProtoStateMachine::DATA_TRANS;

    // Generate first packet.
    SPacket packet;
    packet.startTime = trans.srcTime;
    packet.sourceAddress = trans.sourceAddress;
    packet.destinationAddress = trans.destinationAddress;
    if (sm.protoDesc & SPD_BARRIER) {
        for (int i = 0; i < packet.destinationAddress.size(); i ++) {
            packet.destinationAddress[i] = 0;
        }
    }
    packet.packetSize = trans.packetSize;
    packet.id = trans.id;

    trans_list_.push_back(sm);

    return packet;
}

void ProtoEngine::update_trans(time_type a, const flit_template& flit) {
    for (auto& trans : trans_list_) {
        if (trans.id == flit.getPacketId()) {
            trans.packetDelay.push_back(flit.send_finish_time() - flit.start_time());
            trans.packetDelay.push_back(a - flit.start_time());

            if (trans.protoDesc & SPD_LOCKER) {  // Locker
                update_trans_locker(a, trans);
            } else if (trans.protoDesc & SPD_BARRIER) {  // Barrier
                update_trans_barrier(a, trans);
            } else {
                update_trans_normal(a, trans);
            }

            if (trans.status == ProtoStateMachine::DONE) {
                static ofstream ofs(configuration::wap().delayinfo_fname().c_str());
                ofs << (long)trans.srcTime << ' '
                    /* <<a<<' ' */;
                for (auto& x : trans.sourceAddress) ofs << x << ' ';
                for (auto& x : trans.destinationAddress) ofs << x << ' ';
                ofs << trans.protoDesc << ' ' << trans.packetDelay.size() << ' ';
                for (auto& x : trans.packetDelay) ofs << (long)x << ' ';
                ofs << endl;
            }
        }
    }
}

void ProtoEngine::update_trans_normal(time_type a, ProtoStateMachine& trans) {
    trans.status = ProtoStateMachine::DONE;
}

void ProtoEngine::update_trans_locker(time_type a, ProtoStateMachine& trans) {
    if (trans.status == ProtoStateMachine::DATA_TRANS) {
        // Add new packet.
        SPacket packet;
        packet.startTime = (a > trans.dstTime) ? a : trans.dstTime;
        packet.sourceAddress = trans.destinationAddress;
        packet.destinationAddress = trans.sourceAddress;
        packet.packetSize = 1;
        packet.id = trans.id;
        std::cout << "locker_trans " << packet.startTime << std::endl;

        sim_foundation::wsf().inputTrace(packet);
        sim_foundation::wsf().router(packet.sourceAddress).inputTrace(packet);
        mess_queue::wm_pointer().update_EVG_cycle(a);

        trans.status = ProtoStateMachine::ACK_TRANS;
    } else {
        trans.status = ProtoStateMachine::DONE;
    }
}

void ProtoEngine::update_trans_barrier(time_type a, ProtoStateMachine& trans) {
    if (trans.status == ProtoStateMachine::DATA_TRANS) {
        int uid = trans.destinationAddress[0];
        int count = trans.protoDesc & SPD_BARRIER_COUNT_MASK;

        if (barrier_count_map_.find(uid) == barrier_count_map_.end()) {
            if (count == 0) {
                // Add new packet.
                SPacket packet;
                packet.startTime = a;
                packet.sourceAddress = trans.destinationAddress;
                for (int i = 0; i < packet.sourceAddress.size(); i ++) {
                    packet.sourceAddress[i] = 0;
                }
                packet.destinationAddress = trans.sourceAddress;
                packet.packetSize = 1;
                packet.id = trans.id;
                std::cout << "barrier_trans " << packet.startTime << std::endl;

                sim_foundation::wsf().inputTrace(packet);
                sim_foundation::wsf().router(packet.sourceAddress).inputTrace(packet);
                mess_queue::wm_pointer().update_EVG_cycle(a);

                trans.status = ProtoStateMachine::ACK_TRANS;
            } else if (count == 1) {
                barrier_count_map_[uid] = count;
                barrier_items_map_[uid] = std::vector<int>();

                // Add new packet.
                SPacket packet;
                packet.startTime = a;
                packet.sourceAddress = trans.destinationAddress;
                for (int i = 0; i < packet.sourceAddress.size(); i ++) {
                    packet.sourceAddress[i] = 0;
                }
                packet.destinationAddress = trans.sourceAddress;
                packet.packetSize = 1;
                packet.id = trans.id;
                std::cout << "barrier_trans " << packet.startTime << std::endl;

                sim_foundation::wsf().inputTrace(packet);
                sim_foundation::wsf().router(packet.sourceAddress).inputTrace(packet);
                mess_queue::wm_pointer().update_EVG_cycle(a);

                trans.status = ProtoStateMachine::ACK_TRANS;
            } else {
                barrier_count_map_[uid] = count;
                barrier_items_map_[uid] = std::vector<int>();
                barrier_items_map_[uid].push_back(trans.id);
            }
        } else {
            if (barrier_items_map_.find(uid) == barrier_items_map_.end()) {
                barrier_items_map_[uid] = std::vector<int>();
            }
            barrier_items_map_[uid].push_back(trans.id);

            if (count > 0) {
                barrier_count_map_[uid] = count;
            }

            if (barrier_items_map_[uid].size() >= barrier_count_map_[uid]) {
                // Barrier overflow, send ACK to each item.
                for (int item_id : barrier_items_map_[uid]) {
                    for (auto& trans : trans_list_) {
                        if (trans.id == item_id) {
                            // Add new packet.
                            SPacket packet;
                            packet.startTime = a;
                            packet.sourceAddress = trans.destinationAddress;
                            for (int i = 0; i < packet.sourceAddress.size(); i ++) {
                                packet.sourceAddress[i] = 0;
                            }
                            packet.destinationAddress = trans.sourceAddress;
                            packet.packetSize = 1;
                            packet.id = trans.id;
                            std::cout << "barrier_trans " << packet.startTime << std::endl;

                            sim_foundation::wsf().inputTrace(packet);
                            sim_foundation::wsf().router(packet.sourceAddress).inputTrace(packet);
                            mess_queue::wm_pointer().update_EVG_cycle(a);

                            trans.status = ProtoStateMachine::ACK_TRANS;
                        }
                    }
                }
                barrier_items_map_[uid].clear();
            }
        }
    } else {
        trans.status = ProtoStateMachine::DONE;
    }
}

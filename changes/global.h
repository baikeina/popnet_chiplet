#ifndef GLOBAL_H
#define GLOBAL_H

#include"../index.h"

#include<unordered_set>

extern std::unordered_set<time_type>routing_periods;

/**
 * @brief Behavior descriptor of synchronization protocol.
 */
enum SyncProtocolDesc
{
    /**
     * @brief Acknowledge.
     */
    SPD_ACK = 0x01,
    /**
     * @brief Synchronization before data transmission.
     */
    SPD_PRE_SYNC = 0x02,
    /**
     * @brief Synchronization after data transmission.
     */
    SPD_POST_SYNC = 0x04
};

#endif

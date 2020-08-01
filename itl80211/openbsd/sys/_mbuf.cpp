//
//  _mbuf.cpp
//  itlwm
//
//  Created by qcwap on 2020/6/14.
//  Copyright © 2020 钟先耀. All rights reserved.
//

#include "_mbuf.h"

#include <IOKit/IOCommandGate.h>

extern IOCommandGate *_fCommandGate;

static IOReturn _if_input(OSObject *target, void *arg0, void *arg1, void *arg2, void *arg3)
{
    mbuf_t m;
    bool isEmpty = true;
    struct ifnet *ifq = (struct ifnet *)arg0;
    struct mbuf_list *ml = (struct mbuf_list *)arg1;
    
    MBUF_LIST_FOREACH(ml, m) {
        if (ifq->iface == NULL) {
            panic("%s ifq->iface == NULL!!!\n", __FUNCTION__);
            break;
        }
        if (m == NULL) {
            XYLog("%s m == NULL!!!\n", __FUNCTION__);
            continue;
        }
        //        XYLog("%s %d 啊啊啊啊 ifq->iface->inputPacket(m) hdr_len=%d len=%d\n", __FUNCTION__, __LINE__, mbuf_pkthdr_len(m), mbuf_len(m));
        isEmpty = false;
        ifq->iface->inputPacket(m, 0, IONetworkInterface::kInputOptionQueuePacket);
        if (ifq->netStat != NULL) {
            ifq->netStat->inputPackets++;
        }
		else {
			XYLog("netstat is NULL\n");
		}
    }
    if (!isEmpty) {
        ifq->iface->flushInputQueue();
    }
    return kIOReturnSuccess;
}

int if_input(struct ifnet *ifq, struct mbuf_list *ml)
{
	if (ml_empty(ml))
		return kIOReturnSuccess;

    return _fCommandGate->runAction((IOCommandGate::Action)_if_input, ifq, ml);
}


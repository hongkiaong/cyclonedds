/*
 * Copyright(c) 2021 Apex.AI Inc. All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#ifndef _SHM_monitor_H_
#define _SHM_monitor_H_

#include "iceoryx_binding_c/subscriber.h"
#include "iceoryx_binding_c/listener.h"

#include "dds/ddsrt/threads.h"
#include "dds/ddsrt/sync.h"
#include "dds/ddsi/shm_transport.h"

#if defined (__cplusplus)
extern "C" {
#endif

// ICEORYX_TODO: the iceoryx listener has a maximum number of subscribers that can be registered but this can only be //  queried at runtime
// currently it is hardcoded to be 128 events in the iceoryx C binding
// and we need one registration slot for the wake up trigger
#define SHM_MAX_NUMBER_OF_READERS 127

struct dds_reader;
struct shm_monitor ;

typedef struct {
    iox_user_trigger_storage_t storage;
    struct shm_monitor* monitor;

    //we cannot use those in concurrent wakeups (i.e. only one user callback can be invoked per trigger)
    void (*call) (void*);
    void* arg;
} iox_user_trigger_storage_extension_t;

enum shm_monitor_states {
    SHM_MONITOR_NOT_RUNNING = 0,
    SHM_MONITOR_RUNNING = 1
};

/// @brief abstraction for monitoring the shared memory communication with an internal
///        thread responsible for reacting on received data via shared memory
struct shm_monitor {
    ddsrt_mutex_t m_lock; //currently not needed but we keep it until finalized

    iox_listener_storage_t m_listener_storage;
    iox_listener_t m_listener;

    //use this if we wait but want to wake up for some reason e.g. terminate
    iox_user_trigger_storage_extension_t m_wakeup_trigger_storage;
    iox_user_trigger_t m_wakeup_trigger;

    //TODO: atomics
    uint32_t m_number_of_attached_readers;
    uint32_t m_state;
};

typedef struct shm_monitor shm_monitor_t;

/// @brief initialize the shm_monitor
/// @param monitor self
void shm_monitor_init(shm_monitor_t* monitor);

/// @brief delete the shm_monitor
/// @param monitor self
void shm_monitor_destroy(shm_monitor_t* monitor);

/// @brief wake up the internal listener and invoke a function in the listener thread
/// @param monitor self
/// @param function function to invoke
/// @param arg generic argument for the function (lifetime must be ensured by the user)
dds_return_t shm_monitor_wake_and_invoke(shm_monitor_t* monitor, void (*function) (void*), void* arg);

/// @brief wake up the internal listener and disable execution of listener callbacks
///        due to received data
/// @param monitor self
dds_return_t shm_monitor_wake_and_disable(shm_monitor_t* monitor);

/// @brief wake up the internal listener and enable execution of listener callbacks
///        due to received data
/// @param monitor self
dds_return_t shm_monitor_wake_and_enable(shm_monitor_t* monitor);

/// @brief attach a new reader
/// @param monitor self
/// @param reader reader to attach
dds_return_t shm_monitor_attach_reader(shm_monitor_t* monitor, struct dds_reader* reader);

/// @brief detach a reader
/// @param monitor self
/// @param reader reader to detach
dds_return_t shm_monitor_detach_reader(shm_monitor_t* monitor, struct dds_reader* reader);

// ICEORYX_TODO: clarify lifetime of readers, it should be ok since they are detached in the dds_reader_delete call

#if defined (__cplusplus)
}
#endif
#endif

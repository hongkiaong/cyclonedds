/*
 * Copyright(c) 2021 ADLINK Technology Limited and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#ifndef DDS_SHM__TRANSPORT_H
#define DDS_SHM__TRANSPORT_H

#include "dds/export.h"
#include "iceoryx_binding_c/subscriber.h"
#include "dds/ddsrt/sync.h"
#include "dds/ddsi/q_protocol.h" /* for, e.g., SubmessageKind_t */
#include "dds/ddsi/ddsi_tran.h"
#include "dds/features.h"
#include "dds/ddsi/ddsi_config.h"

#include "dds/ddsi/ddsi_keyhash.h"
#include "iceoryx_binding_c/chunk.h"

#if defined (__cplusplus)
extern "C" {
#endif

typedef enum {
  IOX_CHUNK_UNINITIALIZED,
  IOX_CHUNK_CONTAINS_RAW_DATA,
  IOX_CHUNK_CONTAINS_SERIALIZED_DATA
} iox_shm_data_state_t;

struct iceoryx_header {
   struct ddsi_guid guid;
   dds_time_t tstamp;
   uint32_t statusinfo;
   uint32_t data_size;
   unsigned char data_kind;
   ddsi_keyhash_t keyhash;
   iox_shm_data_state_t shm_data_state;
};

typedef struct iceoryx_header iceoryx_header_t;

struct dds_reader;
struct shm_monitor;

typedef struct {
    iox_sub_storage_t storage;
    //we use a mutex per subscriber to handle concurrent take and release of the data
    ddsrt_mutex_t mutex;
    struct shm_monitor* monitor;
    struct dds_reader* parent_reader;
} iox_sub_storage_extension_t;

//lock and unlock for individual subscribers
DDS_EXPORT void iox_sub_storage_extension_init(iox_sub_storage_extension_t* storage);

DDS_EXPORT void iox_sub_storage_extension_fini(iox_sub_storage_extension_t* storage);

DDS_EXPORT void shm_lock_iox_sub(iox_sub_t sub);

DDS_EXPORT void shm_unlock_iox_sub(iox_sub_t sub);

DDS_EXPORT void free_iox_chunk(iox_sub_t *iox_sub, void **iox_chunk);

DDS_EXPORT iceoryx_header_t *iceoryx_header_from_chunk(const void *iox_chunk);

DDS_EXPORT void shm_set_loglevel(enum ddsi_shm_loglevel);


#if defined (__cplusplus)
}
#endif

#endif //DDS_SHM__TRANSPORT_H

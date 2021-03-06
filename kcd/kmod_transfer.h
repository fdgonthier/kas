/* Copyright (C) 2006-2012 Opersys inc., All rights reserved. */

#ifndef _KMOD_TRANSFER_H
#define _KMOD_TRANSFER_H

#include "kmod_base.h"

/* This object represents a communication driver. */
struct kmod_comm_driver {
    
    /* This function reads data from the remote side. It takes as argument a
     * file descriptor, a buffer where the data is read and an integer which
     * specify the requested number of bytes to transfer on input and the actual
     * number of bytes transferred on output. This function returns 0 on
     * success. If no data is available for reading, this function returns -2.
     * On failure, this function sets the KMO error string and returns -1.
     */
    int (*read_data) (int fd, char *buf, uint32_t *len);
    
    /* Same as above, for sending data. */
    int (*write_data) (int fd, char *buf, uint32_t *len);
    
    /* This function closes the communication descriptor and sets it to -1,
     * if needed.
     */
    void (*disconnect) (int *fd);
};

/* Communication driver for sockets. */
extern struct kmod_comm_driver kmod_sock_driver;

/* Status codes for kmod_data_transfer.  */
enum {
    /* No transfer (have not been added yet to the transfer hub). */
    KMOD_DATA_TRANS_NONE,
    
    /* The transfer has not been completed yet. */
    KMOD_DATA_TRANS_PENDING,
    
    /* The transfer has been completed (min_len bytes have been transfered).
     * Note that if you leave a completed transfer object in the transfer hub,
     * the hub may attempt to transfer more bytes (up to max_len byte). Thus,
     * the status may change again if an error occurs.
     */ 
    KMOD_DATA_TRANS_COMPLETED,
    
    /* An error occurred during the transfer. */
    KMOD_DATA_TRANS_ERROR,
};

/* This object represents a data transfer between two parties. It is meant to
 * be used with the transfer hub.
 */
struct kmod_data_transfer {
    
    /* True if the transfer is a read transfer, as opposed to a write transfer.
     * This field must be set prior to the call to kmod_transfer_hub_add().
     */
    int read_flag;
    
    /* Communication driver for the transfer. This field must be set prior to
     * the call to kmod_transfer_hub_add().
     */
    struct kmod_comm_driver driver;
    
    /* File descriptor used for the transfer. This field must be set prior to
     * the call to kmod_transfer_hub_add().
     */
    int32_t fd;
    
    /* Data buffer. This field must be set prior to the call to
     * kmod_transfer_hub_add().
     */
    char *buf;
    
    /* Miminum number of bytes to transfer for the transfer to be deemed
     * 'completed'. Note that if this field is 0, the transfer is completed as
     * soon as the descriptor becomes readable/writable. This field must be set
     * prior to the call to kmod_transfer_hub_add().
     */
    uint32_t min_len;
    
    /* Maximum number of bytes that can be transferred. This field must be set 
     * prior to the call to kmod_transfer_hub_add().
     */
    uint32_t max_len;
    
    /* Number of bytes that have been transferred so far. This field should
     * not be modified outside the transfer hub. It is initialized by
     * kmod_transfer_hub_add().
     */
    uint32_t trans_len;
    
    /* Operation timeout in milliseconds. '0' means no timeout. Note that
     * the semantic of the timeout is 'maximum delay between each transfer
     * of at least 1 byte'. This field must be set prior to the call to
     * kmod_transfer_hub_add().
     */
    uint32_t op_timeout;
    
    /* Status of the transfer. This field is initialized by
     * kmod_transfer_hub_add().
     */
    int status;
    
    /* Object used internally by the transfer hub to detect situations where a
     * connection timeouts.
     */
    struct timeval deadline;
    
    /* When status == KMOD_DATA_TRANS_ERROR, this object describes the error
     * that occurred. If the pointer is NULL, a timeout occurred. Otherwise, the
     * pointer points to a string containing the error string set by the call to
     * transfer_func(). This field is initialized by kmod_transfer_hub_add().
     */
    kstr *err_msg;
};

/* The transfer hub is used to wait for several transfers at the same time. */
struct kmod_transfer_hub {

    /* Hash containing the current transfers. */
    khash transfer_hash;
};

/* This function returns the error message corresponding to the transfer error
 * that occurred.
 */
static inline char * kmod_data_transfer_err(struct kmod_data_transfer *self) {
    assert(self->status == KMOD_DATA_TRANS_ERROR);
    return (self->err_msg ? self->err_msg->data : "timeout occurred");
}

void kmod_data_transfer_init(struct kmod_data_transfer *self);
void kmod_data_transfer_clean(struct kmod_data_transfer *self);
void kmod_transfer_hub_init(struct kmod_transfer_hub *self);
void kmod_transfer_hub_clean(struct kmod_transfer_hub *self);
void kmod_transfer_hub_add(struct kmod_transfer_hub *hub, struct kmod_data_transfer *transfer);
void kmod_transfer_hub_remove(struct kmod_transfer_hub *hub, struct kmod_data_transfer *transfer);
void kmod_transfer_hub_wait(struct kmod_transfer_hub *hub);


#endif

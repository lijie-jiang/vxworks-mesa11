/* i2c.h - i2c functionality header file*/

/*
 * Copyright (c) 1999-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
21jul15,yat  Clean up vxoal (US63380)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux i2c operations.

NOMANUAL
*/

#ifndef _VXOAL_I2C_H_
#define _VXOAL_I2C_H_

#include <vxoal/krnl/list.h> /* for list_head */
#include <vxoal/krnl/device.h> /* for device, module */

#define I2C_M_TEN               0x0010  /* this is a ten bit chip address */
#define I2C_M_RD                0x0001  /* read data, from slave to master */
#define I2C_M_NOSTART           0x4000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR      0x2000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK        0x1000  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK         0x0800  /* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN          0x0400  /* length will be first received byte */

/*flags for the client struct: */
#define I2C_CLIENT_PEC  0x04            /* Use Packet Error Checking */
#define I2C_CLIENT_TEN  0x10            /* we have a ten bit chip address */
                                        /* Must equal I2C_M_TEN below */
#define I2C_CLIENT_WAKE 0x80            /* for board_info; true iff can wake */
#define I2C_CLIENT_SCCB 0x9000          /* Use Omnivision SCCB protocol */
                                        /* Must match I2C_M_STOP|IGNORE_NAK */

/* i2c adapter classes (bitmask) */
#define I2C_CLASS_HWMON         (1<<0)  /* lm_sensors, ... */
#define I2C_CLASS_DDC           (1<<3)  /* DDC bus on graphics adapters */
#define I2C_CLASS_SPD           (1<<7)  /* Memory modules */

/* Internal numbers to terminate lists */
#define I2C_CLIENT_END          0xfffeU

#define I2C_FUNC_I2C                    0x00000001
#define I2C_FUNC_10BIT_ADDR             0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING      0x00000004 /* I2C_M_IGNORE_NAK etc. */
#define I2C_FUNC_SMBUS_PEC              0x00000008
#define I2C_FUNC_NOSTART                0x00000010 /* I2C_M_NOSTART */
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL  0x00008000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_QUICK            0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE        0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE       0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA   0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA  0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA   0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA  0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL        0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA  0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA 0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK   0x04000000 /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK  0x08000000 /* w/ 1-byte reg. addr. */
#define I2C_FUNC_SMBUS_BYTE             (I2C_FUNC_SMBUS_READ_BYTE | \
                                         I2C_FUNC_SMBUS_WRITE_BYTE)
#define I2C_FUNC_SMBUS_BYTE_DATA        (I2C_FUNC_SMBUS_READ_BYTE_DATA | \
                                         I2C_FUNC_SMBUS_WRITE_BYTE_DATA)
#define I2C_FUNC_SMBUS_WORD_DATA        (I2C_FUNC_SMBUS_READ_WORD_DATA | \
                                         I2C_FUNC_SMBUS_WRITE_WORD_DATA)
#define I2C_FUNC_SMBUS_BLOCK_DATA       (I2C_FUNC_SMBUS_READ_BLOCK_DATA | \
                                         I2C_FUNC_SMBUS_WRITE_BLOCK_DATA)
#define I2C_FUNC_SMBUS_I2C_BLOCK        (I2C_FUNC_SMBUS_READ_I2C_BLOCK | \
                                         I2C_FUNC_SMBUS_WRITE_I2C_BLOCK)

#define I2C_FUNC_SMBUS_EMUL             (I2C_FUNC_SMBUS_QUICK | \
                                         I2C_FUNC_SMBUS_BYTE | \
                                         I2C_FUNC_SMBUS_BYTE_DATA | \
                                         I2C_FUNC_SMBUS_WORD_DATA | \
                                         I2C_FUNC_SMBUS_PROC_CALL | \
                                         I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | \
                                         I2C_FUNC_SMBUS_I2C_BLOCK | \
                                         I2C_FUNC_SMBUS_PEC)


struct i2c_adapter;

struct i2c_msg
    {
    UINT16 addr;        /* slave address */
    UINT16 flags;
    UINT16 len;         /* msg length */
    UINT8 *buf;         /* pointer to msg data */
    };

struct i2c_algo_bit_data
    {
    void *data;             /* private data for lowlevel routines */
    void (*setsda) (void *data, int state);
    void (*setscl) (void *data, int state);
    int  (*getsda) (void *data);
    int  (*getscl) (void *data);
    int  (*pre_xfer)  (struct i2c_adapter *);
    void (*post_xfer) (struct i2c_adapter *);

    /* local settings */
    int udelay;             /* half clock cycle time in us,
                               minimum 2 us for fast-mode I2C,
                               minimum 5 us for standard-mode I2C and SMBus,
                               maximum 50 us for SMBus */
    int timeout;            /* in jiffies */
    };

/*
 * Data for SMBus Messages
 */
#define I2C_SMBUS_BLOCK_MAX     32      /* As specified in SMBus standard */
union i2c_smbus_data
    {
    UINT8  byte;
    UINT16 word;
    UINT8  block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
                               /* and one more for user-space compatibility */
    };

struct i2c_algorithm
    {
    /* If an adapter algorithm can't do I2C-level access, set master_xfer
       to NULL. If an adapter algorithm can do SMBus access, set
       smbus_xfer. If set to NULL, the SMBus protocol is simulated
       using common I2C messages */
    /* master_xfer should return the number of messages successfully
       processed, or a negative value on error */
    int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,
                       int num);
    int (*smbus_xfer) (struct i2c_adapter *adap, UINT16 addr,
                       unsigned short flags, char read_write,
                       UINT8 command, int size, union i2c_smbus_data *data);

    /* To determine what the adapter supports */
    UINT32 (*functionality) (struct i2c_adapter *);
    };

struct i2c_adapter
    {
    struct module *owner;
    unsigned int class;                 /* classes to allow probing for */
    const struct i2c_algorithm *algo;   /* the algorithm to access the bus */
    void *algo_data;

    /* data fields that are valid for all devices */
    int retries;
    struct device dev;      /* the adapter device */
    char name[48];
    };

static inline int i2c_transfer
    (
    struct i2c_adapter *adap,
    struct i2c_msg *msgs,
    int num
    )
    {
    if (adap == NULL)
        return 0;

    if (adap->algo == NULL)
        return 0;

    if (adap->algo->master_xfer == NULL)
        return 0;

    return (adap->algo->master_xfer (adap, msgs, num));
    }

static inline int i2c_add_adapter
    (
    struct i2c_adapter *adap
    )
    {
    return 0;
    }

static void i2c_del_adapter
    (
    struct i2c_adapter *adap
    )
    {
    }

#endif /* _VXOAL_I2C_H_ */

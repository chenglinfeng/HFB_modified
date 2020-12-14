/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-18 15:23:02
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 10:55:00
 * @Description: 
 */
#pragma once

#include <morse/rot/receiver.h>
#include <morse/rot/sender.h>
#include <morse/ctrdrbg/ctrdrbg.h>
#include <morse/ctrdrbg/rdrand.h>

class OTOfflineUtile
{
private:
    unsigned char m_sender_hash_data[99];
    unsigned char m_a[32];
    unsigned char m_aA_point[64];

public:
    /**
     * @description:  生成一个a_half
     * @param {type} []
     * @return: 
     */
    void offline_sender_gen_A_half(unsigned char a_half[33]);

    /**
     * @description: sender 用于的一次OT的key
     * @param {type} []
     * @return: 
     */
    /**
     * @description: receiver 用于的一次OT的key
     * @param {type} []
     * @return: 
     */    
    void offline_sender_gen_key_set(unsigned int *key_set, unsigned int n, unsigned char b_half[33]);
    static unsigned int offline_receiver_gen_key(unsigned char b_half_out[33], unsigned char a_half[33], unsigned int offline_choose, unsigned int n);
};

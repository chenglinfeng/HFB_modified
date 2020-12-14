/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-21 13:31:10
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-21 15:49:20
 * @Description: 
 */
#include "HFB/OTOneUtil.h"

void OTOneUtil::encode_node(unsigned char node_ex[256], unsigned char pack)
{
    unsigned int range = 256;
    unsigned int middel_range = 128;

    /*
        *  range: 0~3  (R+V-R) mod 4 = V; if V in {0,1} result=0; else result=1
        *        sender                      receiver                 result
        *          R                            V-R                     x
        *          1                             0                      0
        *                                        1                      1
        *                                        2                      1
        *                                        3                      0
        *            
        */

    for (int i = 0; i < range; i++)
    {
        unsigned int tmp = (pack + i) % range;
        if (tmp < middel_range)
            node_ex[i] = 0;
        else
            node_ex[i] = 1;
    }
}

void OTOneUtil::calculate_with_share_node_ex(unsigned char node_ex[256], unsigned char share)
{
    for (int i = 0; i < 256; i++)
    {
        node_ex[i] = (node_ex[i] ^ share) & 0x1;
    }
}

void OTOneUtil::encode_nodes(unsigned char **node_exs, unsigned char *pack_shares, unsigned int node_size)
{
    for (unsigned int i = 0; i < node_size; i++)
    {
        encode_node(node_exs[i], pack_shares[i]);
    }
}

void OTOneUtil::calculate_with_shares_node_exs(unsigned char **node_exs, unsigned int node_size,
                                               unsigned char *share, unsigned int share_len)
{
    if (share_len > node_size)
    {
        share_len = node_size;
    }
    for (unsigned int i = 0; i < share_len; i++)
    {
        calculate_with_share_node_ex(node_exs[i], share[i]);
    }
}

void OTOneUtil::compress_b256_b32(unsigned char b32[32], unsigned char b256[256])
{
    for (unsigned int i = 0; i < 32; i++)
    {
        unsigned char compress_item = 0;
        for (unsigned int k = 0; k < 8; k++)
        {
            compress_item ^= b256[i * 8 + k] << k;
        }
        b32[i] = compress_item;
    }
}

void OTOneUtil::online_sender_en_node_ex(unsigned char node_ex[256],
                                         unsigned int *key, unsigned char offset)
{

    // turn up the choice to find the correct key index
    // offset = online - offline
    // key_index = (data_index - offset) mod n
    unsigned int n = 256;
    for (unsigned int j = 0; j < n; j++)
    {
        unsigned int key_index = (j - offset) % n;
        node_ex[j] = (node_ex[j] ^ key[key_index]) & 0x1;
    }
}

void OTOneUtil::online_sender_en_node_exs(unsigned char *compree_node_exs, unsigned char **node_exs, unsigned int *key_sets,
                                          unsigned char *offset, unsigned int ot_size)
{
    unsigned int n = 256;
    for (unsigned int i = 0; i < ot_size; i++)
    {
        online_sender_en_node_ex(node_exs[i], key_sets + i * n, offset[i]);
        compress_b256_b32(compree_node_exs + i * 32, node_exs[i]);
    }
}

unsigned char OTOneUtil::online_receiver_de_node_ex(unsigned int key, unsigned char online_choice,
                                                    unsigned char cmpres_en_node_ex[32])
{
    unsigned int index = online_choice % 8;
    unsigned int block_index = online_choice / 8;

    unsigned char tmp = cmpres_en_node_ex[block_index];
    tmp = (tmp >> index) & 0x01;
    tmp = (tmp ^ key) & 0x01;
    return tmp;
}

unsigned char OTOneUtil::online_receiver_de_node_exs(unsigned char *re_out, unsigned char *cmpres_en_node_ex,
                                                     unsigned char *online_choice, unsigned int *key,
                                                     unsigned int ot_size)
{
    for (unsigned int i = 0; i < ot_size; i++)
    {
        re_out[i] = online_receiver_de_node_ex(key[i], online_choice[i], cmpres_en_node_ex + i * 32);
    }
}
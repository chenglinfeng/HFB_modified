/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-21 13:27:48
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 15:45:18
 * @Description: 
 */

#include "HFB/OTOfflineUtile.h"
void OTOfflineUtile::offline_sender_gen_A_half(unsigned char a_half[33])
{
    N_RandomOT_Sender_Step1(m_sender_hash_data, m_a, a_half, m_aA_point);
}

void OTOfflineUtile::offline_sender_gen_key_set(unsigned int *key_set, unsigned int n, unsigned char b_half[33])
{
    unsigned char **key = new unsigned char *[n];
    for (unsigned int i = 0; i < n; i++)
    {
        key[i] = new unsigned char[32];
    }

    N_RandomOT_Sender_Step2(m_sender_hash_data, key, m_a, b_half, m_aA_point, n);

    for (unsigned i = 0; i < n; i++)
    {
        memcpy(key_set + i, key[i], sizeof(unsigned int));
        delete[] key[i];
    }

    delete key;
}

unsigned int OTOfflineUtile::offline_receiver_gen_key(unsigned char b_half_out[33], unsigned char a_half[33], unsigned int offline_choose, unsigned int n)
{
    unsigned char key_out[32];
    unsigned char A_point[64] = {0};
    unsigned char receiver_hash_data[99] = {0};
    unsigned char b[32] = {0};
    N_RandomOT_Receiver_Step1(receiver_hash_data, offline_choose, b, b_half_out, A_point, a_half, n);
    N_RandomOT_Receiver_Step2(receiver_hash_data, key_out, b, A_point);
    unsigned int tmp = 0;
    memcpy(&tmp, key_out, sizeof(unsigned int));
    return tmp;
}
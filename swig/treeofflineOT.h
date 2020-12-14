/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-12 14:10:48
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 12:49:22
 * @Description: 
 */
#pragma once
#include <HFB/OTOfflineUtile.h>
#include <HFB/OTTwoUtil.h>
#include <HFB/OTThreeUtil.h>

class TreeOfflineOT
{
private:
    OTOfflineUtile offline_ot_sender;
    unsigned int first_layer_size;
    unsigned int middle_layer_size;
    unsigned int end_layer_size;
    unsigned int node_size;
    int height;

public:
    TreeOfflineOT(unsigned int node_size);
    void offline_ot_sender_gen_A_half(unsigned char a_half_out[33]);

    void offline_ot_sender_gen_key_set(unsigned int *key_set, unsigned char b_half[33], unsigned int n);

    void offline_ot_sender_gen_all_key_sets(unsigned int *key_sets, unsigned char *b_halfs, unsigned int ot_size, unsigned int ot_n);

    unsigned int offline_ot_sender_key_sets_size(unsigned int ot_size, unsigned int ot_n);

    unsigned int offline_ot_receiver_offline_choices_size(unsigned int ot_size);

    void offline_ot_receiver_gen_offline_choices_ot_one(unsigned int *offline_choice, unsigned int len, unsigned int ot_n);
    void offline_ot_receiver_gen_offline_choices_ot_two(unsigned int *offline_choice, unsigned int len);
    void offline_ot_receiver_gen_offline_choices_ot_three(unsigned int *offline_choice, unsigned int len, unsigned int ot_n);

    unsigned int offline_ot_receiver_gen_key(unsigned char b_half_out[33],
                                             unsigned char a_half[33], unsigned int offline_choice, unsigned int n);
    void offline_ot_receiver_gen_all_keys(unsigned int *keys, unsigned char a_half[33],
                                          unsigned char *b_halfs, unsigned int *offline_choice, unsigned int ot_size, unsigned int ot_n);

    unsigned int offline_ot_receiver_b_halfs_size(unsigned int ot_size);
    unsigned int offline_ot_receiver_keys_size(unsigned int ot_size);

    unsigned int get_tree_height();

    unsigned int get_first_layer_size();

    unsigned int get_middle_layer_size();

    unsigned int get_end_layer_size();

    int ot_one_get_ot_n();
    int ot_one_get_ot_size();

    int ot_two_get_ot_n();
    int ot_two_get_ot_size();

    int ot_three_get_ot_n();
    int ot_three_get_ot_size();
};

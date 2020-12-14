/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-10 20:26:16
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 11:13:09
 * @Description: 
 */
#pragma once

class TreeUser
{
public:
    unsigned int get_select_share_size(unsigned int node_size);
    void gen_select_random_share(unsigned char *share_out, unsigned int len);
    unsigned int select_in_table(unsigned char *out, unsigned int olen, unsigned int *select_values,
                                 unsigned int *features_index,
                                 unsigned int feature_size, unsigned char *share, unsigned int node_size, char *homo_table_file_name);
    // one ot
    unsigned int ot_one_get_onlice_choices_size(unsigned int ot_size);
    void ot_one_gen_online_choice(unsigned char *online_choices, unsigned char *select_share, unsigned int len);
    void ot_one_gen_offset(unsigned char *offset_out, unsigned char *online_choices, unsigned char *offline_choice, unsigned int len, unsigned int n);
    void ot_one_do(unsigned char *share_out, unsigned char *compress_node_exs,
                   unsigned char *online_choices, unsigned int *key,
                   unsigned int ot_size);

    // two ot
    unsigned int ot_two_get_onlice_choices_size(unsigned int ot_size);
    void ot_two_gen_online_choice(unsigned char *online_choices, unsigned char *ot_one_return_share, int tree_height);
    void ot_two_gen_offset(unsigned char *offset_out, unsigned char *online_choices,
                           unsigned char *offline_choices,
                           unsigned int first_layer_size,
                           unsigned int middle_layer_size,
                           unsigned int end_layer_size);
    void ot_two_do(unsigned char *share_out, unsigned char *en_datas,
                   unsigned int first_layer_size,
                   unsigned int middle_layer_size,
                   unsigned int end_layer_size, unsigned int *keys, unsigned char *online_choices);

    //three ot
    unsigned int ot_three_get_onlice_choices_size(unsigned int ot_size);
    void ot_three_gen_online_choice(unsigned int *online_choices,
                                    unsigned char *ot_two_return_share,
                                    unsigned int first_layer_size,
                                    unsigned int middle_layer_size,
                                    unsigned int end_layer_size,
                                    unsigned int tree_height);
    void ot_three_gen_offset(unsigned int *offset_out,
                             unsigned int *online_choices, unsigned int *offline_choice, unsigned int len, unsigned int n);
    void ot_three_do(unsigned int *random_out, unsigned int *en_datas, unsigned int *online_choices, unsigned int *keys,
                     unsigned int ot_n, unsigned int ot_size);
};

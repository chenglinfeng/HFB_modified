/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-10 20:27:35
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 08:31:48
 * @Description: 
 */
#include "treeuser.h"
#include <string.h>
#include <HFB/strategytb.h>
#include <HFB/sealtable.h>
#include <HFB/OTTwoUtil.h>
#include <HFB/OTThreeUtil.h>
#include <HFB/OTOneUtil.h>

unsigned int TreeUser::get_select_share_size(unsigned int node_size)
{
    return node_size / 2;
}
void TreeUser::gen_select_random_share(unsigned char *share_out, unsigned int len)
{
    srand((unsigned)time(NULL));
    for (size_t i = 0; i < len; i++)
    {
        share_out[i] = rand() % 256;
    }
}

unsigned int TreeUser::select_in_table(unsigned char *out, unsigned int olen, unsigned int *select_values,
                                       unsigned int *features_index,
                                       unsigned int feature_size, unsigned char *share, unsigned int node_size, char *homo_table_file_name)
{
    SealTable t(feature_size, node_size / 2);
    t.load_encrpto_table(homo_table_file_name);
    for (unsigned int i = 0; i < feature_size; i++)
    {
        t.select_uint(features_index[i], select_values[i]);
    }
    unsigned int share_len = node_size / 2;
    t.compute_share(share, share_len);
    t.save_packing_value_string(out, olen);
    t.release_packing_value();
    return olen;
}

unsigned int TreeUser::ot_one_get_onlice_choices_size(unsigned int ot_size)
{
    return ot_size;
}

void TreeUser::ot_one_gen_online_choice(unsigned char *online_choices, unsigned char *select_share, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        online_choices[i] = select_share[i];
    }
}

void TreeUser::ot_one_gen_offset(unsigned char *offset_out, unsigned char *online_choices, unsigned char *offline_choice, unsigned int len, unsigned int n)
{
    for (unsigned int i = 0; i < len; i++)
    {
        offset_out[i] = (online_choices[i] - offline_choice[i]) % n;
    }
}

void TreeUser::ot_one_do(unsigned char *share_out, unsigned char *compress_node_exs,
                         unsigned char *online_choices, unsigned int *key,
                         unsigned int ot_size)
{
    /*############## user receiver do ###########################################*/
    OTOneUtil::online_receiver_de_node_exs(share_out, compress_node_exs, online_choices, key, ot_size);
}

unsigned int TreeUser::ot_two_get_onlice_choices_size(unsigned int ot_size)
{
    return ot_size;
}

void TreeUser::ot_two_gen_online_choice(unsigned char *online_choices, unsigned char *ot_one_return_share, int tree_height)
{
    OTTwoUtil::get_tree_code(online_choices, ot_one_return_share, tree_height);
}

void TreeUser::ot_two_gen_offset(unsigned char *offset_out, unsigned char *online_choices,
                                 unsigned char *offline_choices, 
                                 unsigned int first_layer_size,
                                 unsigned int middle_layer_size,
                                 unsigned int end_layer_size)
{
    OTTwoUtil::calculate_offset(offset_out, first_layer_size, middle_layer_size, end_layer_size, online_choices, offline_choices);
}

void TreeUser::ot_two_do(unsigned char *share_out, unsigned char *en_datas,
                         unsigned int first_layer_size,
                         unsigned int middle_layer_size,
                         unsigned int end_layer_size, unsigned int *keys, unsigned char *online_choices)
{
    unsigned char *first_layer_child_exs = en_datas;
    unsigned char *middle_layer_child_exs = en_datas + first_layer_size;
    unsigned char *end_layer_child_exs = en_datas + first_layer_size + middle_layer_size;

    /*############## user receiver do ###########################################*/
    OTTwoUtil::online_receiver_de_child_exs(share_out,
                                            first_layer_child_exs, first_layer_size,
                                            middle_layer_child_exs, middle_layer_size,
                                            end_layer_child_exs, end_layer_size, keys, online_choices);
}

unsigned int TreeUser::ot_three_get_onlice_choices_size(unsigned int ot_size)
{
    return ot_size;
}

void TreeUser::ot_three_gen_online_choice(unsigned int *online_choices,
                                          unsigned char *ot_two_return_share,
                                          unsigned int first_layer_size,
                                          unsigned int middle_layer_size,
                                          unsigned int end_layer_size,
                                          unsigned int tree_height)
{
    // uncompress path
    int unmp_size = OTThreeUtil::get_uncompress_path_size(first_layer_size, middle_layer_size, end_layer_size);
    unsigned char *uncompress_path_user = new unsigned char[unmp_size];
    OTThreeUtil::uncompress_path(uncompress_path_user, ot_two_return_share, first_layer_size, middle_layer_size, end_layer_size);
    OTThreeUtil::comb_path(online_choices, uncompress_path_user, unmp_size, tree_height);
    delete[] uncompress_path_user;
}

void TreeUser::ot_three_gen_offset(unsigned int *offset_out,
                                   unsigned int *online_choices, unsigned int *offline_choice, unsigned int len, unsigned int n)
{
    for (size_t i = 0; i < len; i++)
    {
        offset_out[i] = (online_choices[i] - offline_choice[i]) % n;
    }
}

void TreeUser::ot_three_do(unsigned int *random_out, unsigned int *en_datas, unsigned int *online_choices, unsigned int *keys,
                           unsigned int ot_n, unsigned int ot_size)
{
    /*############## user receiver do ###########################################*/

    OTThreeUtil::online_receiver_de_path_exs(random_out, en_datas, ot_n, online_choices, keys, ot_size);
}
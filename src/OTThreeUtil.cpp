/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-21 13:39:39
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 15:19:46
 * @Description: 
 */
#include "HFB/OTThreeUtil.h"

int OTThreeUtil::get_end_child_size(unsigned int end_layer_size)
{
    int size = end_layer_size / 8;
    if (size == 0)
    {
        size = 1;
    }
    return size;
}

int OTThreeUtil::get_path_string_size(unsigned int tree_height)
{
    unsigned int tmp = (unsigned int)ceil(((double)tree_height) / 3.0);

    return (0x2U << tmp);
}

int OTThreeUtil::get_uncompress_path_size(unsigned int first_layer_size,
                                          unsigned int middle_layer_size,
                                          unsigned int end_layer_size)
{
    int size = 0;

    if (first_layer_size == 0)
    {
        return 1;
    }

    //first layer
    switch (first_layer_size)
    {
    case 128:
        size = size + 8;
        break;
    case 8:
        size = size + 4;
        break;
    case 2:
        size = size + 2;
        break;
    default:
        break;
    }

    // middle
    unsigned int layers = middle_layer_size / 128;
    size += layers * 8;

    // end  2bit a entity
    unsigned int end_child_size = end_layer_size / 8;
    size += end_child_size;

    return size;
}

void OTThreeUtil::uncompress_path(unsigned char *un_compress_child_share,
                                  unsigned char *child_share,
                                  unsigned int first_layer_size,
                                  unsigned int middle_layer_size,
                                  unsigned int end_layer_size)
{
    unsigned int index = 0;
    unsigned int share_index = 0;

    if (first_layer_size == 0)
    {
        unsigned char tmp = child_share[share_index++];
        un_compress_child_share[index++] = tmp & 0x3U;
        return;
    }

    //first layer
    unsigned char tmp = child_share[share_index++];
    unsigned int first_layer_bit_size = 0;
    switch (first_layer_size)
    {
    case 128:
        first_layer_bit_size = 8;
        break;
    case 8:
        first_layer_bit_size = 4;
        break;
    case 2:
        first_layer_bit_size = 2;
        break;
    default:
        break;
    }

    for (unsigned int i = 0; i < first_layer_bit_size; i++)
    {
        un_compress_child_share[index++] = (tmp >> (first_layer_bit_size - i - 1)) & 0x1;
    }

    // middle
    unsigned int layers = middle_layer_size / 128;

    for (unsigned int i = 0; i < layers; i++)
    {
        unsigned char tmp = child_share[share_index++];
        for (unsigned int j = 0; j < 8; j++)
        {
            un_compress_child_share[index++] = (tmp >> (7 - j)) & 0x1;
        }
    }

    // end  2bit a entity
    unsigned int end_child_size = end_layer_size / 8;
    for (int i = 0; i < end_child_size; i++)
    {
        unsigned char tmp = child_share[share_index++];
        un_compress_child_share[index++] = tmp & 0x3U;
    }
}

void OTThreeUtil::encode_child_path_ex(unsigned int *child_path_ex, unsigned int size,
                                       unsigned int child_path,
                                       unsigned int random_num, unsigned int values[4])
{
    /*
        * 
        *         sender                          (1 -->> n ) receiver                                                         
        *   <s>_i ... <x>_i_1  <x>_i_0         <s>_i ...  <x>_i_1  <x>_i_0           <s>_i... <x>_i_1  <x>_i_0        result
        *      0         0        0                 0      0      0                      0    0    0                  random
        *                                           0      0      1                      0    0    1                  random
        *                                           0      1      0                      0    1    0                  random
        *                                           0      1      1                      0    1    1                  random
        *                                           1      0      0                      1    0    0                v_0 + random
        *                                           1      0      1                      1    0    1                v_1 + random
        *                                           1      1      0                      1    1    0                v_2 + random
        *                                           1      1      1                      1    1    1                v_3 + random
        */
    unsigned int forward = size - 4;
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned int tmp = child_path ^ i;
        if (tmp < forward)
        {
            child_path_ex[i] = random_num;
        }
        else
        {
            child_path_ex[i] = values[(tmp & 0x3UL)] + random_num;
        }
    }
}

void OTThreeUtil::encode_child_path_exs(unsigned int *child_path_ex, unsigned int one_path_string_size,
                                        unsigned int *child_path,
                                        unsigned int *random_num, unsigned int *values, unsigned int ot_size)
{
    for (unsigned int i = 0; i < ot_size; i++)
    {
        encode_child_path_ex(child_path_ex+i*one_path_string_size, one_path_string_size, child_path[i], random_num[i], values+i*4);
    }
}

void OTThreeUtil::online_sender_en_path_ex(unsigned int *child_path_ex, unsigned int size,
                                           unsigned int *key_set, unsigned int offset)
{
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned int index = (i - offset) % size;
        child_path_ex[i] = child_path_ex[i] ^ key_set[index];
    }
}

void OTThreeUtil::online_sender_en_path_exs(unsigned int *child_path_exs, unsigned int one_path_string_size,
                                            unsigned int *key_set, unsigned int *offset, unsigned int ot_size)
{
    for (unsigned int i = 0; i < ot_size; i++)
    {
        online_sender_en_path_ex(child_path_exs+i*one_path_string_size, one_path_string_size, key_set+i*one_path_string_size, offset[i]);
    }
}

unsigned int OTThreeUtil::online_receiver_de_path_ex(unsigned int *en_child_path_ex, unsigned int size,
                                                     unsigned int online_choice, unsigned int key)
{
    return (key ^ en_child_path_ex[online_choice % size]);
}
void OTThreeUtil::online_receiver_de_path_exs(unsigned int *res_out,
                                              unsigned int *en_child_path_exs, unsigned int one_path_string_size,
                                              unsigned int *online_choice, unsigned int *key, unsigned int ot_size)
{
    for (unsigned int i = 0; i < ot_size; i++)
    {
        res_out[i] = online_receiver_de_path_ex(en_child_path_exs+i*one_path_string_size, one_path_string_size, online_choice[i], key[i]);
    }
}

void OTThreeUtil::comb_path(unsigned int *child_path_exs,
                            unsigned char *un_compress_child_share, unsigned int upcslen, unsigned int tree_height)
{
    int tmp = tree_height % 3;
    int child_size = 0;
    int layers = (int)ceil(((double)tree_height) / 3.0) - 2;
    int path_exs_size = 0;

    if (layers == -1) // only one child tree
    {
        child_path_exs[0] = un_compress_child_share[0];
        return;
    }

    switch (tmp)
    {
    case 0:
        child_size = 8;
        break;
    case 2:
        child_size = 4;
        break;
    case 1:
        child_size = 2;
        break;
    default:
        break;
    }

    vector<unsigned int> out;
    vector<unsigned char> tmp_tree;

    for (unsigned int i = 0; i < child_size; i++)
    {
        tmp_tree.push_back(un_compress_child_share[i]);

        unsigned int next_index = 0;
        if (layers == 0)
        {
            next_index = child_size + i;
        }
        else
        {
            next_index = child_size + i * 8;
        }

        dfs(out, un_compress_child_share + next_index, child_size, layers, tmp_tree);

        tmp_tree.pop_back();

        for (unsigned int items : out)
        {
            child_path_exs[path_exs_size++] = items;
        }
        out.clear();
    }
}

void OTThreeUtil::dfs(vector<unsigned int> &out, unsigned char *tree, int gap_size, int deep, vector<unsigned char> &tmp)
{
    unsigned int size = 8;
    if (deep <= 0)
    {
        unsigned int path = 0;
        for (int i = tmp.size() - 1, index = 2; i >= 0; i--, index++)
        {
            path = path ^ (tmp[i] << index);
        }
        path = path ^ (tree[0] & 0x3U);
        out.push_back(path);
        return;
    }

    for (unsigned int i = 0; i < size; i++)
    {
        tmp.push_back(tree[i]);
        unsigned int next_index = 0;
        if (deep == 1)
        {
            next_index = gap_size * size + i;
        }
        else
        {
            next_index = (gap_size + i) * size;
        }
        dfs(out, tree + next_index, gap_size * size, deep - 1, tmp);
        tmp.pop_back();
    }
}
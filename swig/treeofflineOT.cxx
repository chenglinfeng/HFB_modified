/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-12 14:11:35
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 12:52:43
 * @Description: 
 */
#include "treeofflineOT.h"

TreeOfflineOT::TreeOfflineOT(unsigned int tree_node_size)
{
    height = OTTwoUtil::get_tree_height(tree_node_size);
    node_size = tree_node_size;
    OTTwoUtil::get_encode_tree_size(first_layer_size, middle_layer_size, end_layer_size, height);
}

void TreeOfflineOT::offline_ot_sender_gen_A_half(unsigned char a_half_out[33])
{
    offline_ot_sender.offline_sender_gen_A_half(a_half_out);
}

void TreeOfflineOT::offline_ot_sender_gen_key_set(unsigned int *key_set, unsigned char b_half[33], unsigned int n)
{
    offline_ot_sender.offline_sender_gen_key_set(key_set, n, b_half);
}

void TreeOfflineOT::offline_ot_sender_gen_all_key_sets(unsigned int *key_sets, unsigned char *b_halfs, unsigned int ot_size, unsigned int ot_n)
{
    for (size_t i = 0; i < ot_size; i++)
    {
        /* gen key set*/
        offline_ot_sender.offline_sender_gen_key_set(key_sets + i * ot_n, ot_n, b_halfs + i * 33);
    }
}

unsigned int TreeOfflineOT::offline_ot_sender_key_sets_size(unsigned int ot_size, unsigned int ot_n)
{
    return ot_size * ot_n;
}

void TreeOfflineOT::offline_ot_receiver_gen_all_keys(unsigned int *keys, unsigned char a_half[33],
                                                     unsigned char *b_halfs, unsigned int *offline_choice, unsigned int ot_size, unsigned int ot_n)
{
    for (unsigned int i = 0; i < ot_size; i++)
    {
        /* gen key */
        keys[i] = OTOfflineUtile::offline_receiver_gen_key(b_halfs + i * 33, a_half, (unsigned int)offline_choice[i], ot_n);
    }
}

unsigned int TreeOfflineOT::offline_ot_receiver_offline_choices_size(unsigned int ot_size)
{
    return ot_size;
}

void TreeOfflineOT::offline_ot_receiver_gen_offline_choices_ot_one(unsigned int *offline_choice, unsigned int len, unsigned int ot_n)
{
    srand((unsigned)time(NULL));
    for (unsigned int i = 0; i < len; i++)
    {
        offline_choice[i] = rand() % ot_n;
    }
}
void TreeOfflineOT::offline_ot_receiver_gen_offline_choices_ot_two(unsigned int *offline_choice, unsigned int len)
{
    srand((unsigned)time(NULL));
    for (unsigned int i = 0; i < len; i++)
    {
        // offline_choice[i] = rand() % ot_n;
        offline_choice[i] = 0;
    }

    int index = 0;
    if (first_layer_size == 0)
    {
        offline_choice[index] = rand() % end_layer_size;
        return;
    }

    //first layer
    offline_choice[index] = rand() % first_layer_size;
    index++;

    // middle layer
    unsigned int layers = middle_layer_size / 128;
    for (unsigned int i = 0; i < layers; i++)
    {
        offline_choice[index] = rand() % 128;
        index++;
    }

    // end layer
    unsigned int end_child_size = end_layer_size / 8;
    for (unsigned int i = 0; i < end_child_size; i++)
    {
        offline_choice[index] = rand() % 8;
        index++;
    }
}
void TreeOfflineOT::offline_ot_receiver_gen_offline_choices_ot_three(unsigned int *offline_choice, unsigned int len, unsigned int ot_n)
{
    srand((unsigned)time(NULL));
    for (unsigned int i = 0; i < len; i++)
    {
        offline_choice[i] = rand() % ot_n;
    }
}

unsigned int TreeOfflineOT::offline_ot_receiver_gen_key(unsigned char b_half_out[33],
                                                        unsigned char a_half[33], unsigned int offline_choice, unsigned int n)
{
    return OTOfflineUtile::offline_receiver_gen_key(b_half_out, a_half, (unsigned int)offline_choice, n);
}

unsigned int TreeOfflineOT::offline_ot_receiver_b_halfs_size(unsigned int ot_size)
{
    return ot_size * 33;
}

unsigned int TreeOfflineOT::offline_ot_receiver_keys_size(unsigned int ot_size)
{
    return ot_size;
}

unsigned int TreeOfflineOT::get_tree_height()
{
    return height;
}

unsigned int TreeOfflineOT::get_first_layer_size()
{
    return first_layer_size;
}

unsigned int TreeOfflineOT::get_middle_layer_size()
{
    return middle_layer_size;
}

unsigned int TreeOfflineOT::get_end_layer_size()
{
    return end_layer_size;
}

int TreeOfflineOT::ot_one_get_ot_n()
{
    return 256;
}
int TreeOfflineOT::ot_one_get_ot_size()
{
    return node_size / 2;
}

int TreeOfflineOT::ot_two_get_ot_n()
{
    return 128;
}

int TreeOfflineOT::ot_two_get_ot_size()
{
    return OTTwoUtil::get_shares_size(first_layer_size, middle_layer_size, end_layer_size);
}

int TreeOfflineOT::ot_three_get_ot_n()
{
    return OTThreeUtil::get_path_string_size(height);
}

int TreeOfflineOT::ot_three_get_ot_size()
{
    return OTThreeUtil::get_end_child_size(end_layer_size);
}
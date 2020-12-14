/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-19 14:41:16
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 22:22:44
 * @Description: 
 */
#include "test.h"
#include <HFB/OTTwoUtil.h>

unsigned int two_ot_size;
unsigned int two_ot_n = 128;
unsigned int two_ot_offline_choice = 0;
unsigned int *two_ot_key;
unsigned int *two_ot_key_set;

unsigned int first_layer_size;
unsigned int middle_layer_size;
unsigned int end_layer_size;
unsigned int height;

void ot_two_offline()
{

    // get ot size first

    height = OTTwoUtil::get_tree_height(NODE_SIZE);
    OTTwoUtil::get_encode_tree_size(first_layer_size, middle_layer_size, end_layer_size, height);

    two_ot_size = OTTwoUtil::get_shares_size(first_layer_size, middle_layer_size, end_layer_size);

    // END
    OTOfflineUtile sender;
    OTOfflineUtile receiver;

    //sender do
    unsigned char a_half[33] = {0};
    sender.offline_sender_gen_A_half(a_half);

    // receiver do
    unsigned char **b_half;
    b_half = new unsigned char *[two_ot_size];
    two_ot_key = new unsigned int[two_ot_size];
    for (size_t i = 0; i < two_ot_size; i++)
    {
        b_half[i] = new unsigned char[33];

        /* gen key */
        two_ot_key[i] = receiver.offline_receiver_gen_key(b_half[i], a_half, two_ot_offline_choice, two_ot_n);
    }
    /*######################################*/

    //sender do
    two_ot_key_set = new unsigned int [two_ot_size*two_ot_n];
    for (size_t i = 0; i < two_ot_size; i++)
    {
        sender.offline_sender_gen_key_set(two_ot_key_set+i*two_ot_n, two_ot_n, b_half[i]);
    }
    /*######################################*/

    for (size_t i = 0; i < two_ot_size; i++)
    {
        delete[] b_half[i];
    }
    delete[] b_half;
}

unsigned char *two_share;    //mode has
unsigned char *two_re_share; // user has

void ot_two_do()
{
    /*init */
    two_share = new unsigned char[two_ot_size]; // model has
    two_re_share = new unsigned char[two_ot_size];
    memset(two_share, 0, two_ot_size);
    for (size_t i = 0; i < two_ot_size; i++)
    {
        // two_share[i] = rand()%256;
        two_share[i] = 0;
    }
    

    memset(two_re_share, 0, two_ot_size);

    /*############## user do offset ##############*/

    unsigned char *offset = new unsigned char[two_ot_size];
    unsigned char *online_choices = new unsigned char[two_ot_size];
    unsigned char *offline_choices = new unsigned char[two_ot_size];
    for (size_t i = 0; i < two_ot_size; i++)
    {
        offline_choices[i] = two_ot_offline_choice;
    }

    OTTwoUtil::get_tree_code(online_choices, one_re_share, height);
    // cout<<"online" <<endl;
    // for (size_t i = 0; i < two_ot_size; i++)
    // {
    //     cout<< (int)online_choices[i]<<" ";
    // }
    // cout<<endl;
    // cout<<endl;
    

    OTTwoUtil::calculate_offset(offset, first_layer_size, middle_layer_size, end_layer_size, online_choices, offline_choices);

    /*#####################################################################*/

    /*############## mode sender ###########################################*/
    /*############## encode tree #################*/

    unsigned char *first_layer_child_exs = new unsigned char[first_layer_size];
    unsigned char *middle_layer_child_exs = new unsigned char[middle_layer_size];
    unsigned char *end_layer_child_exs = new unsigned char[end_layer_size];

    OTTwoUtil::encode_tree(first_layer_child_exs, middle_layer_child_exs, end_layer_child_exs, one_share, height);

    /*##############   calculate with share #################*/
    OTTwoUtil::calculate_with_shares(first_layer_child_exs, first_layer_size,
                                         middle_layer_child_exs, middle_layer_size,
                                         end_layer_child_exs, end_layer_size, two_share);
    /*############## encrypto node_exs #################*/
    OTTwoUtil::online_sender_en_child_exs(first_layer_child_exs, first_layer_size,
                                              middle_layer_child_exs, middle_layer_size,
                                              end_layer_child_exs, end_layer_size,
                                              two_ot_key_set, offset,two_ot_n);
    /*#####################################################################*/

    /*############## user receiver do ###########################################*/
    OTTwoUtil::online_receiver_de_child_exs(two_re_share,
                                                first_layer_child_exs, first_layer_size,
                                                middle_layer_child_exs, middle_layer_size,
                                                end_layer_child_exs, end_layer_size, two_ot_key, online_choices);

    delete[] offset;
    delete[] online_choices;
    delete[] offline_choices;
    delete[] first_layer_child_exs;
    delete[] middle_layer_child_exs;
    delete[] end_layer_child_exs;

    delete[] two_ot_key;
    delete[] two_ot_key_set;
}

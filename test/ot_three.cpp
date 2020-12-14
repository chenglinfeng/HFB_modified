/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-19 16:36:28
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 22:10:28
 * @Description: 
 */
#include "test.h"
#include <HFB/OTThreeUtil.h>

unsigned int three_ot_size;
unsigned int three_ot_n;
unsigned int three_ot_offline_choice = 0;
unsigned int *three_ot_key;
unsigned int *three_ot_key_set;

void ot_three_offline()
{
    // get ot size first
    three_ot_size = OTThreeUtil::get_end_child_size(end_layer_size);
    // get the ot n
    three_ot_n = OTThreeUtil::get_path_string_size(height);
    // END
    OTOfflineUtile sender;
    OTOfflineUtile receiver;

    //sender do
    unsigned char a_half[33] = {0};
    sender.offline_sender_gen_A_half(a_half);

    // receiver do
    unsigned char **b_half;
    b_half = new unsigned char *[three_ot_size];
    three_ot_key = new unsigned int[three_ot_size];
    for (size_t i = 0; i < three_ot_size; i++)
    {
        b_half[i] = new unsigned char[33];

        /* gen key */
        three_ot_key[i] = receiver.offline_receiver_gen_key(b_half[i], a_half, three_ot_offline_choice, three_ot_n);
    }
    /*######################################*/

    //sender do
    three_ot_key_set = new unsigned int[three_ot_size * three_ot_n];
    for (size_t i = 0; i < three_ot_size; i++)
    {
        sender.offline_sender_gen_key_set(three_ot_key_set + i * three_ot_n, three_ot_n, b_half[i]);
    }
    /*######################################*/

    for (size_t i = 0; i < three_ot_size; i++)
    {
        delete[] b_half[i];
    }
    delete[] b_half;
}

unsigned int *random_num;    //mode has
unsigned int *random_num_re; // user has
unsigned int *values;
void ot_three_do()
{
    /*init */
    random_num = new unsigned int[three_ot_size]; // model has
    memset(random_num, 0, sizeof(int) * three_ot_size);
    for (size_t i = 0; i < three_ot_size; i++)
    {
        random_num[i] = 0;
    }

    random_num_re = new unsigned int[three_ot_size];
    memset(random_num_re, 0, sizeof(int) * three_ot_size);
    values = new unsigned int[three_ot_size * 4];

    // cout<<"three_ot_size "<<three_ot_size<<endl;

    for (size_t i = 0; i < three_ot_size * 4; i++)
    {
        values[i] = i + 1;
    }

    /*############## user do offset ##############*/
    // uncompress path
    int unmp_size = OTThreeUtil::get_uncompress_path_size(first_layer_size, middle_layer_size, end_layer_size);
    // cout << "first_layer_size " << first_layer_size << endl;
    // cout << "middle_layer_size " << middle_layer_size << endl;
    // cout << "end_layer_size " << end_layer_size << endl;

    unsigned char *uncompress_path_user = new unsigned char[unmp_size];
    OTThreeUtil::uncompress_path(uncompress_path_user, two_re_share, first_layer_size, middle_layer_size, end_layer_size);

    // cout << "uncompress_path " << unmp_size << endl;
    // for (size_t i = 0; i < unmp_size; i++)
    // {
    //     // printf("%x : %d \n", &uncompress_path_user[i],uncompress_path_user[i]);
    //     printf("%x ", uncompress_path_user[i]);
    // }
    // cout << endl;

    unsigned int *offset = new unsigned int[three_ot_size];
    unsigned int *online_choices = new unsigned int[three_ot_size];
    OTThreeUtil::comb_path(online_choices, uncompress_path_user, unmp_size, height);

    // cout << "comb_path " << three_ot_size << endl;
    // for (size_t i = 0; i < three_ot_size; i++)
    // {
    //     printf("%x ", online_choices[i]);
    // }
    // cout << endl;

    for (size_t i = 0; i < three_ot_size; i++)
    {
        offset[i] = (online_choices[i] - three_ot_offline_choice) % three_ot_n;
    }
    /*#####################################################################*/

    /*############## mode sender ###########################################*/
    /* recover the tree */
    unsigned char *uncompress_path_model = new unsigned char[unmp_size];
    OTThreeUtil::uncompress_path(uncompress_path_model, two_share, first_layer_size, middle_layer_size, end_layer_size);
    unsigned int *compress_path_mode = new unsigned int[three_ot_size];
    OTThreeUtil::comb_path(compress_path_mode, uncompress_path_model, unmp_size, height);

    //    cout<<"model tree comb_path " <<three_ot_size<<endl;
    //     for (size_t i = 0; i < three_ot_size; i++)
    //     {
    //         cout<<(int)compress_path_mode[i]<<" ";
    //     }
    //     cout<<endl;

    /*##############   encode with random numbers #################*/
    unsigned int *model_paths_encode = new unsigned int[three_ot_size * three_ot_n];

    OTThreeUtil::encode_child_path_exs(model_paths_encode, three_ot_n, compress_path_mode, random_num, values, three_ot_size);

    // cout << "model tree encode_child_path_exs " << three_ot_size << endl;
    // for (size_t i = 0; i < three_ot_size; i++)
    // {
    //     for (size_t j = 0; j < three_ot_n; j++)
    //     {
    //         cout << (int)model_paths_encode[i][j] << " ";
    //     }
    //      cout << endl;
    // }
    // cout << endl;

    /*############## encrypto path_exs #################*/

    OTThreeUtil::online_sender_en_path_exs(model_paths_encode, three_ot_n, three_ot_key_set, offset, three_ot_size);

    /*#####################################################################*/

    /*############## user receiver do ###########################################*/

    OTThreeUtil::online_receiver_de_path_exs(random_num_re, model_paths_encode, three_ot_n, online_choices, three_ot_key, three_ot_size);

   
    delete[] three_ot_key;
    delete[] three_ot_key_set;
    delete[] uncompress_path_user;
    delete[] offset;
    delete[] online_choices;
    delete[] uncompress_path_model;
    delete[] compress_path_mode;

    delete[] model_paths_encode;
}
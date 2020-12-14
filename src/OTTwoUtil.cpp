/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-21 13:34:46
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 17:22:31
 * @Description: 
 */

#include "HFB/OTTwoUtil.h"

void OTTwoUtil::one_layers_child_exs(unsigned char child_exs[2], unsigned char *tree, unsigned int split_index)
{
    /*
        * 
        *         sender                receiver                index (2bits)
        *         <a>_0                  <a>_1         <a>       (x_1,x_2)
        *           0                      0            0           1 0
        *                                  1            1           0 1
        */

    const unsigned char table[2] = {0x2, 0x1};

    /*
        * <a>_0 index : index - 1
        */
    unsigned char a = tree[split_index - 1];

    for (unsigned int i = 0; i < 2; i++)
    {
        child_exs[i] = table[a ^ i];
    }
}

void OTTwoUtil::two_layers_child_exs(unsigned char child_exs[8], unsigned char *tree, unsigned int split_index)
{
    /*
        * 
        *         sender                     receiver                                      leave index (4 bits)
        *  <a>_0  <b>_0  <c>_0         <a>_1  <b>_1  <c>_1           <a>  <b>  <c>           (x_1,x_2,x_3,x_4)
        *    0      0      0             0      0      0              0    0    0               1   0   0   0
        *                                0      0      1              0    0    1               1   0   0   0
        *                                0      1      0              0    1    0               0   1   0   0
        *                                0      1      1              0    1    1               0   1   0   0
        *                                1      0      0              1    0    0               0   0   1   0
        *                                1      0      1              1    0    1               0   0   0   1
        *                                1      1      0              1    1    0               0   0   1   0
        *                                1      1      1              1    1    1               0   0   0   1
        */

    const unsigned char table[8] = {0x8, 0x8, 0x4, 0x4, 0x2, 0x1, 0x2, 0x1};

    /*
        * <a>_0 index : index - 1
        * <b>_0 index : 2*index - 1
        * <c>_0 index : 2*index + 1 -1
        */
    unsigned char a = tree[split_index - 1] << 2;
    unsigned char b = tree[2 * split_index - 1] << 1;
    unsigned char c = tree[2 * split_index];
    unsigned char tmp = a ^ b ^ c;

    for (unsigned int i = 0; i < 8; i++)
    {
        child_exs[i] = table[tmp ^ i];
    }
}
void OTTwoUtil::three_layers_child_exs(unsigned char child_exs[128], unsigned char *tree, unsigned int split_index)
{
    /*
        * 
        *         sender                           receiver (2^7)            index (8 bits)
        *  <a>_0  <b>_0  ...  <g>_0         <a>_1  <b>_1  ...  <g>_1        x_1 x_2  ...  x_8
        *    0      0    ...   0             0      0     ...   0            1   0   ...   0
        *                                    0      0     ...   1            1   0   ...   0
        *                                                  .                          .
        *                                                  .                          .
        *                                                  .                          .
        *                                    1      1           1            0   0   ...   1
        */

    const unsigned char table[128] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,  //8
                                      0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  //16
                                      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,  //24
                                      0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,  //32
                                      0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10, 0x10,  //40
                                      0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10, 0x10,  //48
                                      0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10, 0x10,  //56
                                      0x20, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10, 0x10,  //64
                                      0x08, 0x08, 0x04, 0x04, 0x08, 0x08, 0x04, 0x04,  //72
                                      0x08, 0x08, 0x04, 0x04, 0x08, 0x08, 0x04, 0x04,  //80
                                      0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,  //88
                                      0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,  //96
                                      0x08, 0x08, 0x04, 0x04, 0x08, 0x08, 0x04, 0x04,  //104
                                      0x08, 0x08, 0x04, 0x04, 0x08, 0x08, 0x04, 0x04,  //112
                                      0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,  //116
                                      0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01}; //128
    /*
        * <a>_0 index : index - 1
        * <b>_0 index : 2*index - 1
        * <c>_0 index : 2*index + 1 - 1
        * <d>_0 index : 4*index - 1
        * <e>_0 index : 4*index + 1 - 1
        * <f>_0 index : 4*index + 2 - 1
        * <g>_0 index   4*index + 2 + 1 -1
        */
    unsigned char a = tree[split_index - 1] << 6;
    unsigned char b = tree[2 * split_index - 1] << 5;
    unsigned char c = tree[2 * split_index] << 4;
    unsigned char d = tree[4 * split_index - 1] << 3;
    unsigned char e = tree[4 * split_index] << 2;
    unsigned char f = tree[4 * split_index + 1] << 1;
    unsigned char g = tree[4 * split_index + 2];
    unsigned char tmp = a ^ b ^ c ^ d ^ e ^ f ^ g;

    for (unsigned int i = 0; i < 128; i++)
    {
        child_exs[i] = table[tmp ^ i];
    }
}

void OTTwoUtil::end_two_layers_child_exs(unsigned char child_exs[2], unsigned char *tree, unsigned int split_index)
{
    /*
        * 
        *         sender                receiver                index (1bits)
        *         <a>_0                  <a>_1         <a>         (x_1)
        *           0                      0            0            0
        *                                  1            1            1
        */

    const unsigned char table[2] = {0, 1};

    /*
        * <a>_0 index : index - 1
        * <b>_0 index : 2*index - 1
        * <c>_0 index : 2*index + 1 -1
        */

    unsigned char a = tree[split_index - 1];
    for (unsigned int j = 0; j < 2; j++)
    {
        child_exs[j] = table[a ^ j];
    }
}

void OTTwoUtil::end_three_layers_child_exs(unsigned char child_exs[8], unsigned char *tree, unsigned int split_index)
{
    /*
        * 
        *         sender                     receiver                                      leave index (2 bits)
        *  <a>_0  <b>_0  <c>_0         <a>_1  <b>_1  <c>_1           <a>  <b>  <c>               (x_1,x_0)
        *    0      0      0             0      0      0              0    0    0                  0   0
        *                                0      0      1              0    0    1                  0   0
        *                                0      1      0              0    1    0                  0   1
        *                                0      1      1              0    1    1                  0   1
        *                                1      0      0              1    0    0                  1   0
        *                                1      0      1              1    0    1                  1   1
        *                                1      1      0              1    1    0                  1   0
        *                                1      1      1              1    1    1                  1   1
        */

    const unsigned char table[8] = {0, 0, 1, 1, 2, 3, 2, 3};
    /*
        * <a>_0 index : index - 1
        * <b>_0 index : 2*index - 1
        * <c>_0 index : 2*index + 1 -1
        */

    unsigned char a = tree[split_index - 1] << 2;
    unsigned char b = tree[2 * split_index - 1] << 1;
    unsigned char c = tree[2 * split_index];
    unsigned char tmp = a ^ b ^ c;

    for (unsigned int j = 0; j < 8; j++)
    {
        child_exs[j] = table[tmp ^ j];
    }
}

unsigned char OTTwoUtil::one_layers_encode(unsigned int split_index, unsigned char *tree)
{
    return (tree[split_index - 1]);
}

unsigned char OTTwoUtil::two_layers_encode(unsigned int split_index, unsigned char *tree)
{
    /*
        * <a>_0 index : index - 1
        * <b>_0 index : 2*index - 1
        * <c>_0 index : 2*index + 1 -1
        */
    unsigned char a = tree[split_index - 1] << 2;
    unsigned char b = tree[2 * split_index - 1] << 1;
    unsigned char c = tree[2 * split_index];
    unsigned char tmp = a ^ b ^ c;

    return tmp;
}

unsigned char OTTwoUtil::three_layers_encode(unsigned int split_index, unsigned char *tree)
{
    /*
        * <a>_0 index : index - 1
        * <b>_0 index : 2*index - 1
        * <c>_0 index : 2*index + 1 - 1
        * <d>_0 index : 4*index - 1
        * <e>_0 index : 4*index + 1 - 1
        * <f>_0 index : 4*index + 2 - 1
        * <g>_0 index   4*index + 2 + 1 -1
        */
    unsigned char a = tree[split_index - 1] << 6;
    unsigned char b = tree[2 * split_index - 1] << 5;
    unsigned char c = tree[2 * split_index] << 4;
    unsigned char d = tree[4 * split_index - 1] << 3;
    unsigned char e = tree[4 * split_index] << 2;
    unsigned char f = tree[4 * split_index + 1] << 1;
    unsigned char g = tree[4 * split_index + 2];
    unsigned char tmp = a ^ b ^ c ^ d ^ e ^ f ^ g;

    return tmp;
}

void OTTwoUtil::online_sender_en_child_ex(unsigned char *child_ex, unsigned int clen,
                                          unsigned int *key_set, unsigned char offset)
{
    for (unsigned int i = 0; i < clen; i++)
    {
        unsigned int index = (i - offset) % clen;
        child_ex[i] = (unsigned char)(child_ex[i] ^ key_set[index]);
    }
}

void OTTwoUtil::online_sender_en_child_exs(unsigned char *first_layer_child_exs, unsigned int first_layer_size,
                                           unsigned char *middle_layer_child_exs, unsigned int middle_layer_size,
                                           unsigned char *endl_layer_child_exs, unsigned int end_layer_size,
                                           unsigned int *key_set, unsigned char *offset, unsigned int ot_n)
{
    int index = 0;
    if (first_layer_size == 0)
    {
        online_sender_en_child_ex(endl_layer_child_exs, end_layer_size, key_set, offset[index]);
        return;
    }

    //first layer
    online_sender_en_child_ex(first_layer_child_exs, first_layer_size, key_set, offset[index]);
    index++;

    // middle layer

    unsigned int layers = middle_layer_size / 128;
    for (unsigned int i = 0; i < layers; i++)
    {
        online_sender_en_child_ex(middle_layer_child_exs + i * 128, 128, key_set + index * ot_n, offset[index]);
        index++;
    }

    // end layer
    unsigned int end_child_size = end_layer_size / 8;
    for (int i = 0; i < end_child_size; i++)
    {
        online_sender_en_child_ex(endl_layer_child_exs + i * 8, 8, key_set + index * ot_n, offset[index]);
        index++;
    }
}

unsigned char OTTwoUtil::online_receiver_de_child_ex(unsigned int key, unsigned char online_choice,
                                                     unsigned char *child_ex, unsigned int clen)
{
    unsigned char tmp = child_ex[online_choice % clen];
    tmp = (unsigned char)(tmp ^ key);
    return tmp;
}

void OTTwoUtil::online_receiver_de_child_exs(unsigned char *res_out,
                                             unsigned char *first_layer_child_exs, unsigned int first_layer_size,
                                             unsigned char *middle_layer_child_exs, unsigned int middle_layer_size,
                                             unsigned char *endl_layer_child_exs, unsigned int end_layer_size,
                                             unsigned int *key, unsigned char *online_choice)
{

    int index = 0;
    if (first_layer_size == 0)
    {
        res_out[index] = online_receiver_de_child_ex(key[index], online_choice[index], endl_layer_child_exs, end_layer_size);
        return;
    }

    //first layer
    res_out[index] = online_receiver_de_child_ex(key[index], online_choice[index], first_layer_child_exs, first_layer_size);
    index++;

    // middle layer
    unsigned int layers = middle_layer_size / 128;
    for (unsigned int i = 0; i < layers; i++)
    {
        res_out[index] = online_receiver_de_child_ex(key[index], online_choice[index], middle_layer_child_exs + i * 128, 128);
        index++;
    }

    // end layer
    unsigned int end_child_size = end_layer_size / 8;
    for (int i = 0; i < end_child_size; i++)
    {
        res_out[index] = online_receiver_de_child_ex(key[index], online_choice[index], endl_layer_child_exs + i * 8, 8);
        index++;
    }
}

int OTTwoUtil::get_tree_height(unsigned int tree_size)
{
    return (int)log2(tree_size + 1);
}

void OTTwoUtil::get_encode_tree_size(unsigned int &first_layer_size,
                                     unsigned int &middle_layer_size,
                                     unsigned int &end_layer_size,
                                     int height)
{
    int split_index = 0;
    int tmp = height % 3;

    int layers = (int)ceil(((double)height) / 3.0) - 2;
    if (layers == -1)
    {
        if (tmp == 0)
        {
            first_layer_size = 0;
            middle_layer_size = 0;
            end_layer_size = 8;
        }
        else if (tmp == 2)
        {
            first_layer_size = 0;
            middle_layer_size = 0;
            end_layer_size = 2;
        }
        return;
    }

    switch (tmp)
    {
    case 0:
        first_layer_size = 128;
        split_index = 8;
        break;
    case 1:
        first_layer_size = 2;
        split_index = 2;
        break;
    case 2:
        first_layer_size = 8;
        split_index = 4;
        break;

    default:
        break;
    }

    //fix middle layer
    middle_layer_size = 0;
    for (int i = 0; i < layers; i++)
    {
        middle_layer_size = middle_layer_size + 128 * split_index;
        //update child size  and split_index
        split_index = split_index * 8;
    }

    // finally layer
    end_layer_size = 8 * split_index;
}

void OTTwoUtil::encode_tree(unsigned char *first_layer_child_exs,
                            unsigned char *middle_layer_child_exs,
                            unsigned char *endl_layer_child_exs,
                            unsigned char *tree, int height)
{
    int tmp = height % 3;

    int split_index = 1;

    int layers = (int)ceil(((double)height) / 3.0) - 2;

    if (layers == -1)
    {

        if (tmp == 0)
        {
            end_three_layers_child_exs(endl_layer_child_exs, tree, split_index);
        }
        else if (tmp == 2)
        {
            end_two_layers_child_exs(endl_layer_child_exs, tree, split_index);
        }
        return;
    }

    //fix first_layer
    switch (tmp)
    {
    case 0:
        three_layers_child_exs(first_layer_child_exs, tree, split_index);
        split_index = 8;
        break;
    case 2:
        two_layers_child_exs(first_layer_child_exs, tree, split_index);
        split_index = 4;
        break;
    case 1:
        one_layers_child_exs(first_layer_child_exs, tree, split_index);
        split_index = 2;
        break;
    }

    // reminder all is 128 ,so the height is tell them how many 128 turns
    // fix middle layer

    int index = 0;
    for (int i = 0; i < layers; i++)
    {
        for (int j = 0; j < split_index; j++)
        {
            three_layers_child_exs(middle_layer_child_exs + index, tree, split_index + j);
            index += 128;
        }
        //update child size  and split_index
        split_index = split_index * 8;
    }

    // finally layer
    for (int i = 0; i < split_index; i++)
    {
        end_three_layers_child_exs(endl_layer_child_exs + i * 8, tree, split_index + i);
    }
}

void OTTwoUtil::calculate_with_share(unsigned char *child_ex, unsigned int clen,
                                     unsigned char share)
{
    for (unsigned int i = 0; i < clen; i++)
    {
        child_ex[i] ^= share;
    }
}

int OTTwoUtil::get_shares_size(unsigned int first_layer_size,
                               unsigned int middle_layer_size,
                               unsigned int end_layer_size)
{
    //first layer
    int share_index = 1;
    if (first_layer_size != 0)
    {
        // middle layer
        share_index = share_index + middle_layer_size / 128;
        // end layer
        share_index = share_index + end_layer_size / 8;
    }

    return share_index;
}

void OTTwoUtil::calculate_with_shares(unsigned char *first_layer_child_exs, unsigned int first_layer_size,
                                      unsigned char *middle_layer_child_exs, unsigned int middle_layer_size,
                                      unsigned char *endl_layer_child_exs, unsigned int end_layer_size,
                                      unsigned char *shares)
{
    int share_index = 0;
    if (first_layer_size == 0)
    {
        calculate_with_share(endl_layer_child_exs, end_layer_size, shares[share_index++]);
        return;
    }

    //first layer

    calculate_with_share(first_layer_child_exs, first_layer_size, shares[share_index++]);

    // middle layer
    unsigned int layers = middle_layer_size / 128;
    for (unsigned int i = 0; i < layers; i++)
    {
        calculate_with_share(middle_layer_child_exs + i * 128, 128, shares[share_index++]);
    }

    // end layer
    unsigned int end_child_size = end_layer_size / 8;
    for (int i = 0; i < end_child_size; i++)
    {
        calculate_with_share(endl_layer_child_exs + i * 8, 8, shares[share_index++]);
    }
}

void OTTwoUtil::get_tree_code(unsigned char *child_code,
                              unsigned char *tree,
                              unsigned int tree_height)
{
    int tmp = tree_height % 3;
    int index = 0;
    int split_index = 1;

    int layer = (int)ceil(((double)tree_height) / 3.0) - 2;

    if (layer == -1) // one tree
    {
        if (tmp == 0)
        {
            child_code[index] = two_layers_encode(split_index, tree);
        }
        else if (tmp == 2)
        {
            child_code[index] = one_layers_encode(split_index, tree);
        }
        return;
    }

    // fix first tree
    switch (tmp)
    {
    case 0:
        child_code[index++] = three_layers_encode(split_index, tree);
        split_index = 8;
        break;
    case 1:
        child_code[index++] = one_layers_encode(split_index, tree);
        split_index = 2;
        break;
    case 2:
        child_code[index++] = two_layers_encode(split_index, tree);
        split_index = 4;
        break;
    default:
        break;
    }

    // fix middle layer
    for (int i = 0; i < layer; i++)
    {
        for (int j = 0; j < split_index; j++)
        {
            child_code[index++] = three_layers_encode(split_index + j, tree);
        }

        //update child size  and split_index
        split_index = split_index * 8;
    }

    // finally layer
    for (int i = 0; i < split_index; i++)
    {
        child_code[index++] = two_layers_encode(split_index + i, tree);
    }
}

void OTTwoUtil::calculate_offset(unsigned char *offset,
                                 unsigned int first_layer_size,
                                 unsigned int middle_layer_size,
                                 unsigned int end_layer_size,
                                 unsigned char *online_choices,
                                 unsigned char *offline_choices)
{

    int index = 0;
    if (first_layer_size == 0)
    {
        offset[index] = (online_choices[index] - offline_choices[index]) % end_layer_size;
        return;
    }

    //first layer

    offset[index] = (online_choices[index] - offline_choices[index]) % first_layer_size;
    index++;

    // middle layer
    unsigned int layers = middle_layer_size / 128;
    for (unsigned int i = 0; i < layers; i++)
    {
        offset[index] = (online_choices[index] - offline_choices[index]) % 128;
        index++;
    }

    // end layer
    unsigned int end_child_size = end_layer_size / 8;
    for (int i = 0; i < end_child_size; i++)
    {
        offset[index] = (online_choices[index] - offline_choices[index]) % 8;
        index++;
    }
}
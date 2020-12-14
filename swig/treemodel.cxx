/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-10 20:16:30
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 10:47:37
 * @Description: 
 */
#include "treemodel.h"
#include <string.h>
#include <HFB/strategytb.h>
#include <HFB/sealtable.h>
#include <HFB/OTTwoUtil.h>
#include <HFB/OTThreeUtil.h>
#include <HFB/OTOneUtil.h>
#include <stdlib.h>
#include <time.h>

void TreeModel::generate_homo_table(unsigned int *features_value, unsigned int *features_set, unsigned int node_size, unsigned int features_size,
                                    char *tb_file_name, char *pk_file_name, char *pr_file_name)
{
    StrategyTable table;
    table.init(features_size);
    for (unsigned int i = 0; i < node_size / 2; i++)
    {
        table.insert_compare_uint(OPERATE::EBIGGER, features_set[i], features_value[i]); // 根据feature值于feature下标生成表
    }

    SealTable t(table);
    t.gen_keypair(); //生成公私匙
    t.encrypto_table(table); //加密表
    t.save_encrypto_table(tb_file_name);//存储表
    t.save_secret_key(pr_file_name); //存储公钥
    t.save_public_key(pk_file_name); //存储私钥
}

unsigned int TreeModel::get_pack_size(unsigned int node_size)
{
    return node_size / 2;
}

void TreeModel::decrypto_packing_value(unsigned char *pack_out, unsigned int pack_size, unsigned char *en_pack_value, unsigned int en_pack_size, char *pr_file_name)
{
    SealTable t(0, 0, 0, 0);
    t.load_secret_key(pr_file_name); //加载私钥
    t.load_packing_value_string(en_pack_value, en_pack_size); // 加载 加密的packing值
    t.decrypto_packing_value(pack_out, pack_size); // 解密并输出结果到pack_out
}

void TreeModel::ot_one_gen_random_share(unsigned char *share_out, unsigned int on_leaves_node_size)
{
    srand((unsigned)time(NULL));
    for (unsigned int i = 0; i < on_leaves_node_size; i++)
    {
        share_out[i] = rand() % 2;
    }
}

unsigned int TreeModel::ot_one_do_out_size(unsigned int node_size)
{
    return node_size * 16;
}

void TreeModel::ot_one_do(unsigned char *out, unsigned char *pack_value, unsigned char *share,
                          unsigned int node_size,
                          unsigned int *key_sets,
                          unsigned char *offset)
{

    unsigned int on_leaves_node_size = node_size / 2;
    unsigned char **node_exs = new unsigned char *[on_leaves_node_size];
    for (size_t i = 0; i < on_leaves_node_size; i++)
    {
        node_exs[i] = new unsigned char[256];
    }
    /*############## encode share #################*/
    // 构建真值表， 根据预测值的另一个share
    OTOneUtil::encode_nodes(node_exs, pack_value, on_leaves_node_size);

    /*##############   calculate with share #################*/
    // 盲化第一轮OT的真值表
    OTOneUtil::calculate_with_shares_node_exs(node_exs, on_leaves_node_size, share, on_leaves_node_size);

    /*############## encrypto node_exs and compress #################*/
    // 加密构造好的真值表
    OTOneUtil::online_sender_en_node_exs(out, node_exs, key_sets, offset, on_leaves_node_size);

    for (size_t i = 0; i < on_leaves_node_size; i++)
    {
        delete[] node_exs[i];
    }
    delete[] node_exs;
}

unsigned int TreeModel::ot_one_get_share_size(unsigned int ot_size)
{
    return ot_size;
}

unsigned int TreeModel::ot_two_get_share_size(unsigned int ot_size)
{
    return ot_size;
}

void TreeModel::ot_two_gen_random_share(unsigned char *share_out, unsigned int len)
{
    srand((unsigned)time(NULL));
    for (unsigned int i = 0; i < len; i++)
    {
        share_out[i] = rand() % 256;
    }
}

unsigned int TreeModel::ot_two_do_out_size(unsigned int first_layer_size,
                                           unsigned int middle_layer_size,
                                           unsigned int end_layer_size)
{
    return first_layer_size + middle_layer_size + end_layer_size;
}

void TreeModel::ot_two_do(unsigned char *out, unsigned char *ot_one_shares, unsigned char *shares,
                          unsigned int *key_sets,
                          unsigned char *offset,
                          unsigned int first_layer_size,
                          unsigned int middle_layer_size,
                          unsigned int end_layer_size,
                          int tree_height, unsigned int ot_n)
{
    /*############## encode tree #################*/
    unsigned char *first_layer_child_exs = out;
    unsigned char *middle_layer_child_exs = out + first_layer_size;
    unsigned char *end_layer_child_exs = out + first_layer_size + middle_layer_size;

    // 进行树的拆分，然后构建真值表
    OTTwoUtil::encode_tree(first_layer_child_exs, middle_layer_child_exs, end_layer_child_exs, ot_one_shares, tree_height);

    /*##############   calculate with share #################*/
    // 盲化OT的真值表
    OTTwoUtil::calculate_with_shares(first_layer_child_exs, first_layer_size,
                                     middle_layer_child_exs, middle_layer_size,
                                     end_layer_child_exs, end_layer_size, shares);
    /*############## encrypto node_exs #################*/
    // 加密构造好的真值表
    OTTwoUtil::online_sender_en_child_exs(first_layer_child_exs, first_layer_size,
                                          middle_layer_child_exs, middle_layer_size,
                                          end_layer_child_exs, end_layer_size,
                                          key_sets, offset, ot_n);
    /*#####################################################################*/
}

unsigned int TreeModel::ot_three_get_random_nums_size(unsigned int ot_size)
{
    return ot_size;
}
void TreeModel::ot_three_gen_random_nums(unsigned int *nums, unsigned int len)
{
    srand((unsigned)time(NULL));
    for (unsigned int i = 0; i < len; i++)
    {
        nums[i] = 0;
    }
}

unsigned int TreeModel::ot_three_do_out_size(unsigned int tree_height,
                                             unsigned int end_layer_size)
{
    return OTThreeUtil::get_path_string_size(tree_height) * OTThreeUtil::get_end_child_size(end_layer_size);
}

void TreeModel::ot_three_do(unsigned int *out, unsigned char *ot_two_shares, unsigned int *random_nums,
                            unsigned int *values,
                            unsigned int *key_sets, unsigned int *offset,
                            unsigned int first_layer_size,
                            unsigned int middle_layer_size,
                            unsigned int end_layer_size,
                            unsigned int tree_height)
{
    /*############## mode sender ###########################################*/
    unsigned int ot_size = OTThreeUtil::get_end_child_size(end_layer_size);
    unsigned int ot_n = OTThreeUtil::get_path_string_size(tree_height);
    /* recover the tree */
   
    int unmp_size = OTThreeUtil::get_uncompress_path_size(first_layer_size, middle_layer_size, end_layer_size);

    unsigned char *uncompress_path_model = new unsigned char[unmp_size];
     // 第二轮OT的子树的share，由1比特扩充为1个字节存储，为了后面组合路径更加方便，可以利用数组下标寻值
    OTThreeUtil::uncompress_path(uncompress_path_model, ot_two_shares, first_layer_size, middle_layer_size, end_layer_size);
  
    unsigned int *compress_path_mode = new unsigned int[ot_size];
    // 构建真值表，组合路径，利用回溯的深度优先遍历
    OTThreeUtil::comb_path(compress_path_mode, uncompress_path_model, unmp_size, tree_height);

    /*##############   encode with random numbers #################*/
    // 用随机值掩盖叶子节点的值
    OTThreeUtil::encode_child_path_exs(out, ot_n, compress_path_mode, random_nums, values, ot_size);

    /*############## encrypto path_exs #################*/
    // 加密构造好的真值表
    OTThreeUtil::online_sender_en_path_exs(out, ot_n, key_sets, offset, ot_size);

    /*#####################################################################*/
}

unsigned int TreeModel::ot_three_get_values_size(unsigned int ot_size)
{
    return ot_size * 4;
}
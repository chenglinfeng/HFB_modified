/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-10 20:11:57
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 10:40:21
 * @Description: 
 */
#pragma once

class TreeModel
{
public:
    /**
     * @description: 生成同态查找表
     * @param features_value ：feature值
     * @param features_set   ：feature值的下表
     * @param node_size   ：树的总节点个数
     * @param features_size   ：feature值的数组大小
     * @param tb_file_name   ：同态查找表的文件名
     * @param pk_file_name   ：公钥文件名
     * @param pr_file_name   ：私钥文件名    
     * @return: 
     */
    void generate_homo_table(unsigned int *features_value, unsigned int *features_set, unsigned int node_size, unsigned int features_size,
                             char *tb_file_name, char *pk_file_name, char *pr_file_name);

    /**
     * @description: 获取 解密的 packing 值的大小 
     * @param {type} []
     * @return: 
     */
    unsigned int get_pack_size(unsigned int node_size);

    /**
     * @description: 解密 packing 值
     * @param {type} []
     * @return: 
     */
    void decrypto_packing_value(unsigned char *pack_out, unsigned int pack_size, unsigned char *en_pack_value, unsigned int en_pack_size, char *pr_file_name);

    // one ot
    unsigned int ot_one_get_share_size(unsigned int ot_size);
    void ot_one_gen_random_share(unsigned char *share_out, unsigned int len);
    unsigned int ot_one_do_out_size(unsigned int node_size);
    void ot_one_do(unsigned char *out, unsigned char *pack_value, unsigned char *share,
                   unsigned int node_size,
                   unsigned int *key_sets,
                   unsigned char *offset);

    // two ot
    unsigned int ot_two_get_share_size(unsigned int ot_size);
    void ot_two_gen_random_share(unsigned char *share_out, unsigned int len);
    unsigned int ot_two_do_out_size(unsigned int first_layer_size,
                                    unsigned int middle_layer_size,
                                    unsigned int end_layer_size);
    void ot_two_do(unsigned char *out, unsigned char *ot_one_shares, unsigned char *shares,
                   unsigned int *key_sets,
                   unsigned char *offset,
                   unsigned int first_layer_size,
                   unsigned int middle_layer_size,
                   unsigned int end_layer_size,
                   int tree_height, unsigned int ot_n);

    //three ot
    unsigned int ot_three_get_values_size(unsigned int ot_size);
    unsigned int ot_three_get_random_nums_size(unsigned int ot_size);
    void ot_three_gen_random_nums(unsigned int *nums, unsigned int len);
    unsigned int ot_three_do_out_size(unsigned int tree_height,
                                      unsigned int end_layer_size);
    void ot_three_do(unsigned int *out, unsigned char *ot_two_shares, unsigned int *random_nums,
                     unsigned int *values,
                     unsigned int *key_sets, unsigned int *offset,
                     unsigned int first_layer_size,
                     unsigned int middle_layer_size,
                     unsigned int end_layer_size,
                     unsigned int tree_height);
};

/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-17 17:17:21
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 11:06:47
 * @Description: 
 */

#pragma once

#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

class OTThreeUtil
{

public:
    /**
     * @description: 获取拆分树后，最后一层由多少个树
     * @param {type} []
     * @return: 
     */
    static int get_end_child_size(unsigned int end_layer_size);

    /**
     * @description: 组合路径的长度
     * @param {type} []
     * @return: 
     */
    static int get_path_string_size(unsigned int tree_height);

    /**
     * @description:  第二轮OT的子树的share，由1比特扩充为1个字节存储的长度
     * @param {type} []
     * @return: 
     */
    static int get_uncompress_path_size(unsigned int first_layer_size,
                                        unsigned int middle_layer_size,
                                        unsigned int end_layer_size);

    /**
     * @description: 
     * 第二轮OT的子树的share，由1比特扩充为1个字节存储，为了后面组合路径更加方便，可以利用数组下标寻值
     * @param {type} []
     * @return: 
     */
    static void uncompress_path(unsigned char *un_compress_child_share,
                                unsigned char *child_share,
                                unsigned int first_layer_size,
                                unsigned int middle_layer_size,
                                unsigned int end_layer_size);

    /**
     * @description: 用随机值掩盖一个子树的叶子节点的值
     * @param {type} []
     * @return: 
     */
    static void encode_child_path_ex(unsigned int *child_path_ex, unsigned int size,
                                     unsigned int child_path,
                                     unsigned int random_num, unsigned int values[4]);

    /**
     * @description: 用随机值掩盖所有子树的叶子节点的值
     * @param {type} []
     * @return: 
     */
    static void encode_child_path_exs(unsigned int *child_path_ex, unsigned int one_path_string_size,
                                      unsigned int *child_path,
                                      unsigned int *random_num, unsigned int *values, unsigned int ot_size);

    /**
     * @description: 加密构造好的一个路径的真值表
     * @param {type} []
     * @return: 
     */
    static void online_sender_en_path_ex(unsigned int *child_path_ex, unsigned int size,
                                         unsigned int *key_set, unsigned int offset);

    /**
     * @description: 加密构造好的所有路径的真值表
     * @param {type} []
     * @return: 
     */
    static void online_sender_en_path_exs(unsigned int *child_path_exs, unsigned int one_path_string_size,
                                          unsigned int *key_set, unsigned int *offset, unsigned int ot_size);

    /**
     * @description: 获取一个路径的随机数掩盖的叶子值
     * @param {type} []
     * @return: 
     */
    static unsigned int online_receiver_de_path_ex(unsigned int *en_child_path_ex, unsigned int size,
                                                   unsigned int online_choice, unsigned int key);
    /**
     * @description: 获取所有路径的随机数掩盖的所有叶子值
     * @param {type} []
     * @return: 
     */
    static void online_receiver_de_path_exs(unsigned int *res_out,
                                            unsigned int *en_child_path_exs, unsigned int one_path_string_size,
                                            unsigned int *online_choice, unsigned int *key, unsigned int ot_size);

    /**
     * @description: 构建真值表，组合路径，利用回溯的深度优先遍历
     * @param {type} []
     * @return: 
     */
    static void comb_path(unsigned int *child_path_exs,
                          unsigned char *un_compress_child_share, unsigned int upcslen, unsigned int tree_height);

    /**
     * @description: 回溯的深度优先遍历
     * @param {type} []
     * @return: 
     */
    static void dfs(vector<unsigned int> &out, unsigned char *tree, int gap_size, int deep, vector<unsigned char> &tmp);
};

/*
 * @Author: xin.xing.lzj
 * @Date: 2019-07-29 14:11:05
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 11:13:59
 * @Description: 
 */
#pragma once

#include <cmath>
#include <iostream>
using namespace std;

class OTTwoUtil
{
public:
    /**
     * @description: 只有一层的分支真值表的编码
     * @param {type} []
     * @return: 
     */
    static void one_layers_child_exs(unsigned char child_exs[2], unsigned char *tree, unsigned int split_index);

    /**
     * @description: 只有两层的分支真值表的编码
     * @param {type} []
     * @return: 
     */
    static void two_layers_child_exs(unsigned char child_exs[8], unsigned char *tree, unsigned int split_index);
   
   /** 只有三层的分支真值表的编码
     * @description: 
     * @param {type} []
     * @return: 
     */
    static void three_layers_child_exs(unsigned char child_exs[128], unsigned char *tree, unsigned int split_index);

    /**
     * @description: 带叶子的最后一层子树的真值表的编码，子树为两层
     * @param {type} []
     * @return: 
     */
    static void end_two_layers_child_exs(unsigned char child_exs[2], unsigned char *tree, unsigned int split_index);

    /**
     * @description: 带叶子的最后一层子树的真值表的编码，子树为三层
     * @param {type} []
     * @return: 
     */
    static void end_three_layers_child_exs(unsigned char child_exs[8], unsigned char *tree, unsigned int split_index);

    /**
     * @description: 一层的分支编码
     * @param {type} []
     * @return: 
     */
    static unsigned char one_layers_encode(unsigned int split_index, unsigned char *tree);

    /**
     * @description: 两层的分支编码
     * @param {type} []
     * @return: 
     */
    static unsigned char two_layers_encode(unsigned int split_index, unsigned char *tree);
    
    /**
     * @description: 三层的分支编码
     * @param {type} []
     * @return: 
     */
    static unsigned char three_layers_encode(unsigned int split_index, unsigned char *tree);

    /**
     * @description: 加密一个子树的真值表
     * @param {type} []
     * @return: 
     */
    static void online_sender_en_child_ex(unsigned char *child_ex, unsigned int clen,
                                          unsigned int *key_set, unsigned char offset);

    /**
     * @description: 加密全部子树的真值表
     * @param {type} []
     * @return: 
     */
    static void online_sender_en_child_exs(unsigned char *first_layer_child_exs, unsigned int first_layer_size,
                                           unsigned char *middle_layer_child_exs, unsigned int middle_layer_size,
                                           unsigned char *endl_layer_child_exs, unsigned int end_layer_size,
                                           unsigned int *key_set, unsigned char *offset,unsigned int ot_n);
    /**
     * @description: 获取一个OT的子树的share
     * @param {type} []
     * @return: 
     */
    static unsigned char online_receiver_de_child_ex(unsigned int key, unsigned char online_choice,
                                                     unsigned char *child_ex, unsigned int clen);

    /**
     * @description: 获取全部OT的子树的share
     * @param {type} []
     * @return: 
     */
    static void online_receiver_de_child_exs(unsigned char *res_out,
                                             unsigned char *first_layer_child_exs, unsigned int first_layer_size,
                                             unsigned char *middle_layer_child_exs, unsigned int middle_layer_size,
                                             unsigned char *endl_layer_child_exs, unsigned int end_layer_size,
                                             unsigned int *key, unsigned char *online_choice);
    
    /**
     * @description: 获取树高 
     * @param {type} []
     * @return: 
     */
    static int get_tree_height(unsigned int tree_size);

    /** 获取拆分树后，真值表数组的大小
     * @description: 
     * @param {type} []
     * @return: 
     */
    static void get_encode_tree_size(unsigned int &first_layer_size,
                                     unsigned int &middle_layer_size,
                                     unsigned int &end_layer_size,
                                     int height);

    /**
     * @description: 拆分树，整个树的真值表编码
     * @param {type} []
     * @return: 
     */
    static void encode_tree(unsigned char *first_layer_child_exs,
                            unsigned char *middle_layer_child_exs,
                            unsigned char *endl_layer_child_exs,
                            unsigned char *tree, int height);

    /**
     * @description: 盲化OT的一个真值表
     * @param {type} []
     * @return: 
     */                        
    static void calculate_with_share(unsigned char *child_ex, unsigned int clen,
                                     unsigned char share);

    static int get_shares_size(unsigned int first_layer_size,
                               unsigned int middle_layer_size,
                               unsigned int end_layer_size);
    /**
     * @description: 盲化OT的全部真值表
     * @param {type} []
     * @return: 
     */
    static void calculate_with_shares(unsigned char *first_layer_child_exs, unsigned int first_layer_size,
                                      unsigned char *middle_layer_child_exs, unsigned int middle_layer_size,
                                      unsigned char *endl_layer_child_exs, unsigned int end_layer_size,
                                      unsigned char *shares);
    /**
     * @description: 获取整个树的编码
     * @param {type} []
     * @return: 
     */
    static void get_tree_code(unsigned char *child_code,
                              unsigned char *tree,
                              unsigned int tree_height);

    /**
     * @description: 计算整个树的offset
     * @param {type} []
     * @return: 
     */
    static void calculate_offset(unsigned char *offset,
                                 unsigned int first_layer_size,
                                 unsigned int middle_layer_size,
                                 unsigned int end_layer_size,
                                 unsigned char *online_choices,
                                 unsigned char *offline_choices);
};
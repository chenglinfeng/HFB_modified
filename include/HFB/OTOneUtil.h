/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-17 11:25:17
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-23 11:00:25
 * @Description: 
 */

#pragma once

class OTOneUtil
{
public:
    /**
     * @description: 根据查表后的值，对一个节点进行真值表编码，0表示向左，0表示向右
     * @param {type} []
     * @return: 
     */
    static void encode_node(unsigned char node_ex[256], unsigned char pack);

    /**
     * @description: 对一个节点进行真值表编码后，进行share的计算，模2加
     * @param {type} []
     * @return: 
     */
    static void calculate_with_share_node_ex(unsigned char node_ex[256], unsigned char share);

    /**
     * @description: 对全部非叶子节点进行真值表编码
     * @param {type} []
     * @return: 
     */
    static void encode_nodes(unsigned char **node_exs, unsigned char *pack_shares, unsigned int node_size);

    /**
     * @description: 对全部非叶子节点的真值表进行share的计算
     * @param {type} []
     * @return: 
     */
    static void calculate_with_shares_node_exs(unsigned char **node_exs, unsigned int node_size,
                                               unsigned char *share, unsigned int share_len);

    /**
     * @description: 压缩函数，将256字节，压缩成32字节
     * @param {type} []
     * @return: 
     */
    static void compress_b256_b32(unsigned char b32[32], unsigned char b256[256]);

    /**
     * @description: 加密一个节点的真值表
     * @param {type} []
     * @return: 
     */
    static void online_sender_en_node_ex(unsigned char node_ex[256],
                                         unsigned int *key, unsigned char offset);

    /**
     * @description: 加密全部非叶子节点的真值表
     * @param {type} []
     * @return: 
     */
    static void online_sender_en_node_exs(unsigned char *compree_node_exs, unsigned char **node_exs,
                                          unsigned int *key_sets,
                                          unsigned char *offset, unsigned int ot_size);

    /**
     * @description: 获取一次OT的或一个节点的值，根据自己的choice
     * @param {type} []
     * @return: 
     */                                      
    static unsigned char online_receiver_de_node_ex(unsigned int key, unsigned char online_choice,
                                                    unsigned char cmpres_en_node_ex[32]);

    /**
     * @description: 获取全部OT的或全部非叶子节点的值，根据自己的全部choice
     * @param {type} []
     * @return: 
     */
    static unsigned char online_receiver_de_node_exs(unsigned char *re_out, unsigned char *cmpres_en_node_ex,
                                                     unsigned char *online_choice, unsigned int *key,
                                                     unsigned int ot_size);
};

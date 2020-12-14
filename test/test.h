/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-19 13:36:25
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-22 22:25:13
 * @Description: 
 */

#pragma once
#include <HFB/OTOfflineUtile.h>

#include <string.h>

#define NODE_SIZE (8*2 -1)
extern unsigned char node_shares[NODE_SIZE/2];

extern unsigned char packing_value_en[16457];
extern unsigned int packing_value_en_len;


extern unsigned int first_layer_size;
extern unsigned int middle_layer_size;
extern unsigned int end_layer_size;
extern unsigned int height;

//#######################
extern unsigned char *one_share ;    //mode has
extern unsigned char *one_re_share; // user has
void ot_one_offline();
void ot_one_do();


//#######################
extern unsigned char *two_share ;    //mode has
extern unsigned char *two_re_share; // user has
void ot_two_offline();
void ot_two_do();

extern unsigned int two_ot_size;


//#######################
extern unsigned int *random_num;    //mode has
extern unsigned int *random_num_re; // user has
extern unsigned int *values;
void ot_three_offline();
void ot_three_do();

extern unsigned int three_ot_size;
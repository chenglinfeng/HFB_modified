#!/usr/bin/env python
# -*- coding: utf-8 -*
'''
@Author: xin.xing.lzj
@Date: 2019-08-12 11:51:35
@LastEditors: xinxing.lzj
@LastEditTime: 2019-08-23 15:18:18
@Description: 
'''
import time
import treePredict as t
import random
# *2*2*2*2*2*2*2*2
node_size  = 8*2 - 1

user = t.TreeUser()
model = t.TreeModel()
offline_ot = t.TreeOfflineOT(node_size)

en_packing_value = t.bytesArray(16457)
en_packing_value_len = 16457

select_share_len = user.get_select_share_size(node_size)
select_share = t.bytesArray(select_share_len)

a_half = t.bytesArray(33)

# 生成同态查找表，由模型方调用
def gen_en_table():
    # 计算非叶子节点的个数：总节点个数 / 2
    size = node_size / 2

    # 存放feature的值，比如 x > 1, feature值为1
    features = t.uintsArray(size)

    # 存放每个feature值的feature下表，如上面feature值为1的是 feature0的值
    features_index = t.uintsArray(size)

    for i in range(node_size / 2):
        features[i] = i + 1
        features_index[i] = i
        
    # 调用模型方的方法：生成同态查找表
    # 参数：
    # feature值
    # feature值的下表
    # 树的总节点个数
    # feature值的数组大小
    # 同态查找表的文件名
    # 公钥文件名
    # 私钥文件名    
    model.generate_homo_table(features,features_index,node_size,size,"en_table","pk_file","pr_file")
    pass


# 用户根据同态查找表进行选择，即进行预测
def user_select():
    size = node_size / 2

    # 存放用户预测值对应的feature下表
    features_index = t.uintsArray(size)
    # 存放用户预测值
    select_values = t.uintsArray(size)
    
    for i in range(size):
        features_index[i] = i
        select_values[i] = i+1
        # if i%2 == 0:
        #     select_values[i] = 0
        # else:
        #     select_values[i] = i+1

    # gen share
    # 生成一个随机的share，用于盲化用户每个节点的判定结果
    user.gen_select_random_share(select_share,select_share_len)
    
    # do select
    # 进行查表和盲化结果
    # 参数：
    # 输出结果
    # 输出结果长度
    # 预测值
    # 预测值对应的feature下表
    # feature值的个数
    # share值
    # 总节点个数
    # 同态查找表文件名
    user.select_in_table(en_packing_value,en_packing_value_len,select_values,features_index,size,select_share,node_size,"en_table")
    pass


# 第一轮OT，离线OT，用于生成在线OT要使用的key
def ot_one_offline_ot_do():
    # 在线OT交互的个数
    ot_size = offline_ot.ot_one_get_ot_size()
    # 每一个OT的范围 n
    ot_n = offline_ot.ot_one_get_ot_n()
    # 存放所有 receiver 使用到的key 
    keys = t.uintsArray(ot_size)
    # 存放所有 sender 使用到的key 
    key_sets = t.uintsArray(ot_n*ot_size)

    # 存放所有 receiver 使用到的离线choice
    offline_choices = t.uintsArray(ot_size)
   
    # 生成一个a_half， 可以多次使用
    offline_ot.offline_ot_sender_gen_A_half(a_half)

    #receiver gen keys
    # receiver 生成在线OT所使用的所有的key
    b_halfs_size = offline_ot.offline_ot_receiver_b_halfs_size(ot_size)
    b_halfs = t.bytesArray(b_halfs_size)
    offline_ot.offline_ot_receiver_gen_offline_choices_ot_one(offline_choices,ot_size,ot_n)
    offline_ot.offline_ot_receiver_gen_all_keys(keys,a_half,b_halfs,offline_choices,ot_size,ot_n)

    #sender gen all key sets
    # sender 生成在线OT所使用的所有的key
    offline_ot.offline_ot_sender_gen_all_key_sets(key_sets,b_halfs,ot_size,ot_n)

    # 兼容接口的处理
    offline_choices_byte = t.bytesArray(ot_size)
    for i in range(ot_size):
        offline_choices_byte[i] = offline_choices[i]

    return offline_choices_byte,keys,key_sets,ot_n,ot_size
    

# 第一轮在线OT
# 参数：
# offline_choices ： 离线的OTchoice
# keys:              receiver 在线OT需要用的key
# key_sets:          sender 在线OT需要用的key
# ot_n:              每一个OT的范围 n
# ot_size：          需要OT交互的次数
def ot_one_do(offline_choices,keys,key_sets,ot_n,ot_size):

    # 计算share的大小，根据需要OT交互的次数来计算
    tmp_len = user.ot_one_get_onlice_choices_size(ot_size)

    # user gen offset first
    # 存放每一个OT的offset值，用于key轮转
    offset = t.bytesArray(tmp_len)
    # 存放用户的online choice
    online_choice = t.bytesArray(tmp_len)
    
    # 根据用户的预测值，构建用户的online choice
    user.ot_one_gen_online_choice(online_choice,select_share,select_share_len)

    # 根据 离线choice 和在线choice 计算offset
    user.ot_one_gen_offset(offset,online_choice,offline_choices,tmp_len,ot_n)

    # model sender
    # decrypto en_packing value
    # 模型方解密预测值的另一个share，即获得预测值的另一个share
    pack_len = model.get_pack_size(node_size)
    pack_value = t.bytesArray(pack_len)
    model.decrypto_packing_value(pack_value,pack_len,en_packing_value,en_packing_value_len,"pr_file")

    # gen share
    # 生成一个随机的share，用于盲化第一轮OT的真值表
    share_len = model.ot_one_get_share_size(ot_size)
    share = t.bytesArray(share_len)
    model.ot_one_gen_random_share(share,share_len)


    # encrypto data
    # 存储模型方加密构造好的真值表，真值表的构造是根据pack_value（预测值的另一个share）来构建的
    en_data_len = model.ot_one_do_out_size(node_size)
    en_data = t.bytesArray(en_data_len)

    # 参数：
    # en_data：     （输出） 加密后的真值表，用于OT交互
    # pack_value ：  模型方获得预测值的另一个share
    # share ：       模型方随机生成的随机share
    # key_sets ：    加密真值表的密钥
    # offset   ：    用于OT密钥轮转
    model.ot_one_do(en_data,pack_value,share,node_size,key_sets,offset)

    # user receiver
    # 用户根据自己的online choice 获得每个节点的share，online choice是由预测值的share构建而来
    share_out = t.bytesArray(tmp_len)

    # 参数：
    # share_out ：结果输出
    # en_data：   模型方发来的加密后的真值表
    # online_choice ： 用户的在线choice
    # keys ：          解密真值表的key
    # ot_size ：       OT交互的次数
    user.ot_one_do(share_out,en_data,online_choice,keys,ot_size)
    
    return share, share_out, tmp_len
    


# 第二轮OT，离线OT，用于生成在线OT要使用的key
def ot_two_offline_ot_do():
    # 在线OT交互的个数
    ot_size = offline_ot.ot_two_get_ot_size()
    # 每一个OT的范围 n
    ot_n = offline_ot.ot_two_get_ot_n()
    # 存放所有 receiver 使用到的key 
    keys = t.uintsArray(ot_size)
     # 存放所有 sender 使用到的key 
    key_sets = t.uintsArray(ot_n*ot_size)

    # 存放所有 receiver 使用到的离线choice
    offline_choices = t.uintsArray(ot_size)

    # 不要生成一个a_half， 直接使用第一个生成的

    #receiver gen keys
    # receiver 生成在线OT所使用的所有的key
    b_halfs_size = offline_ot.offline_ot_receiver_b_halfs_size(ot_size)
    b_halfs = t.bytesArray(b_halfs_size)
    offline_ot.offline_ot_receiver_gen_offline_choices_ot_two(offline_choices,ot_size)
    offline_ot.offline_ot_receiver_gen_all_keys(keys,a_half,b_halfs,offline_choices,ot_size,ot_n)

    #sender gen all key sets
    # sender 生成在线OT所使用的所有的key
    offline_ot.offline_ot_sender_gen_all_key_sets(key_sets,b_halfs,ot_size,ot_n)

    # 兼容接口的处理
    offline_choices_byte = t.bytesArray(ot_size)
    for i in range(ot_size):
        offline_choices_byte[i] = offline_choices[i]

    return offline_choices_byte,keys,key_sets,ot_n,ot_size
    

# 第二轮在线OT
# 参数：
# offline_choices ： 离线的OTchoice
# keys:              receiver 在线OT需要用的key
# key_sets:          sender 在线OT需要用的key
# ot_n:              每一个OT的范围 n
# ot_size：          需要OT交互的次数
# per_share_sender： 模型方的上一轮OT的share
# per_share_receiver：用户方的上一轮OT的share
def ot_two_do(offline_choices,keys,key_sets,ot_n,ot_size,per_share_sender,per_share_receiver):
    # 获取树的高度
    tree_height = offline_ot.get_tree_height()
    # 获取拆分树后，第一个层由多少个分支
    first_layer_size= offline_ot.get_first_layer_size()
    # 获取拆分树后，中间层由多少个树，8叉树
    middle_layer_size = offline_ot.get_middle_layer_size()
    # 获取拆分树后，最后一层由多少个树
    end_layer_size = offline_ot.get_end_layer_size()

    # 计算share的大小，根据需要OT交互的次数来计算
    tmp_len = user.ot_two_get_onlice_choices_size(ot_size)

    # user gen offset first
    # 存放每一个OT的offset值，用于key轮转
    offset = t.bytesArray(tmp_len)
    # 存放用户的online choice
    online_choice = t.bytesArray(tmp_len)

    # 根据用户上一轮的OT的share，构建用户的online choice
    user.ot_two_gen_online_choice(online_choice,per_share_receiver,tree_height)

    # 根据 离线choice 和在线choice 计算offset
    user.ot_two_gen_offset(offset,online_choice,offline_choices,first_layer_size,middle_layer_size,end_layer_size)

    # model sender
    # gen share
    # 生成一个随机的share，用于盲化OT的真值表
    share_len = model.ot_two_get_share_size(ot_size)
    share = t.bytesArray(share_len)
    model.ot_two_gen_random_share(share,share_len)

    # encrypto data
    # 存储模型方加密构造好的真值表，真值表的构造是模型方上一轮OT的share来构建的
    en_data_len = model.ot_two_do_out_size(first_layer_size,middle_layer_size,end_layer_size)
    en_data = t.bytesArray(en_data_len)

    # 参数：
    # en_data：     （输出） 加密后的真值表，用于OT交互
    # pack_value ：  模型方获得预测值的另一个share
    # per_share_sender ： 模型方上一轮OT的share
    # share ：       模型方随机生成的随机share
    # key_sets ：    加密真值表的密钥
    # offset   ：    用于OT密钥轮转
    # first_layer_size ： 拆分树后，第一个层由多少个分支
    # middle_layer_size :拆分树后，中间层由多少个树
    # end_layer_size : 拆分树后，最后一层由多少个树
    # tree_height : 树的高度
    # ot_n : 每一个OT的范围 n
    model.ot_two_do(en_data,per_share_sender,share,key_sets,offset,first_layer_size,middle_layer_size,end_layer_size,tree_height,ot_n)

    # user receiver
    share_out = t.bytesArray(tmp_len)
    # 用户根据自己的online choice 获得每个子树的share
    # 参数：
    # share_out ：结果输出
    # en_data：   模型方发来的加密后的真值表
    # first_layer_size ： 拆分树后，第一个层由多少个分支
    # middle_layer_size :拆分树后，中间层由多少个树
    # end_layer_size : 拆分树后，最后一层由多少个树
    # keys ：          解密真值表的key
    # online_choice ： 用户的在线choice
    user.ot_two_do(share_out,en_data,first_layer_size,middle_layer_size,end_layer_size,keys,online_choice)
    
    return share, share_out, tmp_len


# 第三轮OT，离线OT，用于生成在线OT要使用的key
def ot_three_offline_ot_do():
    # 在线OT交互的个数
    ot_size = offline_ot.ot_three_get_ot_size()
    # 每一个OT的范围 n
    ot_n = offline_ot.ot_three_get_ot_n()
    # 存放所有 receiver 使用到的key 
    keys = t.uintsArray(ot_size)
    # 存放所有 sender 使用到的key 
    key_sets = t.uintsArray(ot_n*ot_size)
    # 存放所有 receiver 使用到的离线choice
    offline_choices = t.uintsArray(ot_size)

    # 不要生成一个a_half， 直接使用第一个生成的

    
    #receiver gen keys
    # receiver 生成在线OT所使用的所有的key
    b_halfs_size = offline_ot.offline_ot_receiver_b_halfs_size(ot_size)
    b_halfs = t.bytesArray(b_halfs_size)
    offline_ot.offline_ot_receiver_gen_offline_choices_ot_three(offline_choices,ot_size,ot_n)
    offline_ot.offline_ot_receiver_gen_all_keys(keys,a_half,b_halfs,offline_choices,ot_size,ot_n)

    #sender gen all key sets
    # sender 生成在线OT所使用的所有的key
    offline_ot.offline_ot_sender_gen_all_key_sets(key_sets,b_halfs,ot_size,ot_n)

    return offline_choices,keys,key_sets,ot_n,ot_size


# 第三轮在线OT
# 参数：
# offline_choices ： 离线的OTchoice
# keys:              receiver 在线OT需要用的key
# key_sets:          sender 在线OT需要用的key
# ot_n:              每一个OT的范围 n
# ot_size：          需要OT交互的次数
# per_share_sender： 模型方的上一轮OT的share
# per_share_receiver：用户方的上一轮OT的share
def ot_three_do(offline_choices,keys,key_sets,ot_n,ot_size,per_share_sender,per_share_receiver):
    # 获取树的高度
    tree_height = offline_ot.get_tree_height()
    # 获取拆分树后，第一个层由多少个分支（2，4，8）
    first_layer_size= offline_ot.get_first_layer_size()
    # 获取拆分树后，中间层由多少个树，8叉树
    middle_layer_size = offline_ot.get_middle_layer_size()
    # 获取拆分树后，最后一层由多少个树
    end_layer_size = offline_ot.get_end_layer_size()

    # 计算share的大小，根据需要OT交互的次数来计算
    tmp_len = user.ot_three_get_onlice_choices_size(ot_size)

    # user gen offset first
    # 存放每一个OT的offset值，用于key轮转
    offset = t.uintsArray(tmp_len)
     # 存放用户的online choice
    online_choice = t.uintsArray(tmp_len)
    # 根据用户上一轮的OT的share，构建用户的online choice
    user.ot_three_gen_online_choice(online_choice,per_share_receiver,first_layer_size,middle_layer_size,end_layer_size,tree_height)
    # 根据 离线choice 和在线choice 计算offset
    user.ot_three_gen_offset(offset,online_choice,offline_choices,tmp_len,ot_n)


    # model sender
    # gen random numbers
    # 为每一个带叶子的子树生成一个随机的值，用于掩盖叶子节点的值
    random_len = model.ot_three_get_random_nums_size(ot_size)
    random_num = t.uintsArray(random_len)
    model.ot_three_gen_random_nums(random_num,random_len)

    # init leaves values
    # 初始化每个叶子的叶子值
    leaves_values_len = model.ot_three_get_values_size(ot_size)
    leaves_values = t.uintsArray(leaves_values_len)
    for i in range(leaves_values_len):
        leaves_values[i] = i + 1

    # encrypto data
    # 存储模型方加密构造好的真值表，真值表的构造是模型方上一轮OT的share来构建的
    en_data_len= model.ot_three_do_out_size(tree_height,end_layer_size)
    en_data = t.uintsArray(en_data_len)
    # 参数：
    # en_data：          （输出） 加密后的真值表，用于OT交互
    # pack_value ：       模型方获得预测值的另一个share
    # per_share_sender ： 模型方上一轮OT的share
    # random_num ：       每一个带叶子的子树的随机的值
    # leaves_values：     叶子值
    # key_sets ：         加密真值表的密钥
    # offset   ：    用于OT密钥轮转
    # first_layer_size ： 拆分树后，第一个层由多少个分支
    # middle_layer_size :拆分树后，中间层由多少个树
    # end_layer_size : 拆分树后，最后一层由多少个树
    # tree_height : 树的高度
    model.ot_three_do(en_data,per_share_sender,random_num,leaves_values,key_sets,offset,first_layer_size,middle_layer_size,end_layer_size,tree_height)

    # user receiver
    # 用户根据自己的online choice 获得每个带叶子的子树的随机数掩盖的值
    # 参数：
    # random_out ：结果输出
    # en_data：   模型方发来的加密后的真值表
    # online_choice ： 用户的在线choice
    # keys ：          解密真值表的key
    # ot_n:              每一个OT的范围 n
    # ot_size：          需要OT交互的次数
    random_out = t.uintsArray(tmp_len)
    user.ot_three_do(random_out,en_data,online_choice,keys,ot_n,ot_size)
    
    return random_num, random_out, tmp_len
   

if __name__ == '__main__':
    # 生成同态查找表
    start = time.time()
    gen_en_table()
    end = time.time()
    print "生成同态查找表 :",
    print end-start
    # 用户根据同态查找表进行选择，即进行预测
    start = time.time()
    user_select()
    end = time.time()
    print "查找同态查找表 :",
    print end-start

    # ot one offline ot
    # 第一轮OT，离线OT，用于生成在线OT要使用的key
    start = time.time()
    offline_choices,keys,key_sets,ot_n,ot_size = ot_one_offline_ot_do()
    end = time.time()
    print "第一轮OT，离线OT :",
    print end-start
    # ot one do
    # 第一轮在线OT
    start = time.time()
    share_sender, share_receiver, tmp_len = ot_one_do(offline_choices,keys,key_sets,ot_n,ot_size)
    end = time.time()
    print "第一轮在线OT :",
    print end-start
    # print "one : "
    # print "sender: "
    # for i in range(tmp_len):
    #     print share_sender[i],
    # print ""
    # print "receiver: "
    # for i in range(tmp_len):
    #     print share_receiver[i],
    # print ""
    # print "combine:"
    # for i in range(tmp_len):
    #     print share_receiver[i]^share_sender[i],
    # print ""

    # ot two offline ot
    # 第二轮OT，离线OT，用于生成在线OT要使用的key
    start = time.time()
    offline_choices,keys,key_sets,ot_n,ot_size = ot_two_offline_ot_do()
    end = time.time()
    print "第二轮OT，离线OT :",
    print end-start
    # ot two do
    # 第二轮在线OT
    start = time.time()
    share_sender, share_receiver, tmp_len = ot_two_do(offline_choices,keys,key_sets,ot_n,ot_size,share_sender,share_receiver)
    end = time.time()
    print "第二轮在线OT :",
    print end-start
    # print "two : "
    # print "sender: "
    # for i in range(tmp_len):
    #     print('%#x'%share_sender[i]),
    # print ""
    # print "receiver: "
    # for i in range(tmp_len):
    #     print('%#x'%share_receiver[i]),
    # print ""

    # ot three offline ot
    # 第三轮OT，离线OT，用于生成在线OT要使用的key
    start = time.time()
    offline_choices,keys,key_sets,ot_n,ot_size = ot_three_offline_ot_do()
    end = time.time()
    print "第三轮OT，离线OT :",
    print end-start
    # ot three do
    # 第三轮在线OT
    start = time.time()
    share_sender, share_receiver, tmp_len = ot_three_do(offline_choices,keys,key_sets,ot_n,ot_size,share_sender,share_receiver)
    end = time.time()
    print "第三轮在线OT :",
    print end-start
    # print "three : "
    # print "sender: "
    # for i in range(tmp_len):
    #     print share_sender[i],
    # print ""
    print "receiver: "
    for i in range(tmp_len):
        print share_receiver[i],
    print ""

    pass

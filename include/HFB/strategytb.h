/*
 * @Author: xin.xing.lzj
 * @Date: 2019-06-30 12:32:31
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-08 17:16:00
 * @Description:  This is a head for plaintext strategy table
 */

#pragma once
#include <vector>
#include <stdint.h>
#include <cstring>
#include <iostream>

using namespace std;

/**
 * @description: 
 *     Converts floating value to unsigned integer. it is a different from the casting into unsigned int. 
 * @param[in] value The floating value
 * @return: 
 *     the converted interger value
 */
inline unsigned int float_trans_uint(float value)
{
    unsigned int tmp = 0;

    memcpy(&tmp, &value, sizeof(float));

    if (tmp >> 31)
        tmp = (~tmp);
    else
        tmp = (tmp | 0x80000000);

    return tmp;
}

/**
 * @description: 
 *     Converts signed int value to unsigned integer. it is a different from the casting into unsigned int. 
 * @param[in] value The signed int value 
 * @return: 
 *     the converted interger value
 */
inline unsigned int int_trans_uint(int value)
{
    unsigned int tmp = value ^ 0xFFFFFFFF;
    tmp = tmp ^ 0x7FFFFFFF;
    return tmp;
}

/**
 * @description: 
 * operator for strategy table
 */
enum class OPERATE : unsigned int
{
    BIGGER = 0x0,   /*bigger than >*/
    EBIGGER = 0x1,  /*no less than >=*/
    SMALLER = 0x2,  /*smaller than <*/
    ESMALLER = 0x3, /*no bigger than <=*/
    EQUAL = 0x4,    /*equal ==*/
};

/**
 * @description: 
 *  strategy table class that the main responsibility is to generate a plaintext strategy table
 */
class StrategyTable
{

protected:
    unsigned int m_branch;      /*the branch size of a fix value such as an unsigned int x can be divided into x[0]...x[7]*/
    unsigned int m_block_size;  /*the size of decision condition, here we will use fixed size 1024 for seal library*/
    unsigned int m_items;       /*the size of each branch, for example, x[0] is a 4 bits number, the totall number of items is 2^4 = 16*/
    unsigned int m_log_items;   /*the logarithmic of size of each branch, for example, m_items is 16, so m_log_items is 4*/
    unsigned char ***m_table;   /*the table data */
    unsigned int m_block_index; /*the current index for the table*/
    unsigned int m_features;    /*the number of features */

private:
    /**
     * @description: 
     *      create a vector of decision condition by operator(EBIGGER or BIGGER)
     * @param[in] value The value of decision condition
     * @param[in] feature_index  index of features
     * @param[in] has_equal A flag in which true is EBIGGER otherwise is BIGGER
     * @return: 
     */
    void create_vector_bigger(unsigned int value, unsigned int feature_index, bool has_equal);

    /**
     * @description: 
     *      create a vector of decision condition by operator(SMALLER or ESMALLER)
     * @param[in] value The value of decision condition
     * @param[in] feature_index  index of features
     * @param[in] has_equal A flag in which true is ESMALLER otherwise is SMALLER
     * @return: 
     */
    void create_vector_smaller(unsigned int value, unsigned int feature_index, bool has_equal);

    /**
     * @description: 
     *  create a vector of decision condition by operator(EQUAL)
     * @param[in] value The value of decision condition
     * @param[in] feature_index  index of features
     * @return: 
     */
    void create_vector_equal(unsigned int value, unsigned int feature_index);

public:
    /**
     * @description: 
     *    get the number of features
     * @return: 
     *    the number of features
     */
    inline unsigned int get_features()
    {
        return m_features;
    }
    /**
     * @description: 
     *  get log_2{items}
     * @return: 
     *     log_2{items}
     */
    inline unsigned int get_log_items()
    {
        return m_log_items;
    }

    /**
     * @description: 
     *  get the branch size
     * @return: 
     *     the size of branch
     */
    inline unsigned int get_branch()
    {
        return m_branch;
    }

    /**
     * @description: 
     *  get the size of items
     * @return: 
     *     the size of items
     */
    inline unsigned int get_items()
    {
        return m_items;
    }

    /**
     * @description: 
     *    get the size of block
     * @return: 
     *      the size of block
     */
    inline unsigned int get_block_size()
    {
        return m_block_size;
    }

    /**
     * @description: 
     *    get the table data
     * @return: 
     *    the const refrence of table
     */
    inline const unsigned char ***get_table() const
    {
        return (const unsigned char ***)m_table;
    }

    /**
     * @description: 
     *     get the actual size of table, or an index of the table
     * @return: 
     *     the index of table
     */
    inline unsigned int size() const
    {
        return m_block_index;
    }

    StrategyTable() = default;

    /**
     * @description: 
     *      Initialize function 
     * @param[in] features      @see m_features
     * @param[in] branch        @see m_branch default is 8 
     * @param[in] block_size    @see m_block_size default is 1024
     * @param[in] items         @see m_items defalut is 16
     * @return: 
     */
    void init(unsigned int num_decision_nodes, unsigned int features, unsigned int branch = 8, unsigned int block_size = 1024, unsigned int items = 16);

    /**
     * @description: 
     *     The constructor that initializes content;
     *       m_branch default is 8 ;
     *       m_block_size default is 1024 ;
     *       m_items defalut is 16 ;
     * @param[in] features      @see m_features
     * @return: 
     */
    StrategyTable(unsigned int num_decision_nodes, unsigned int features);

    /**
     * @description: 
     *     The constructor that initializes content
     * @param[in] features      @see m_features
     * @param[in] branch        @see m_branch
     * @param[in] block_size    @see m_block_size
     * @param[in] items         @see m_items
     * @return: 
     */
    StrategyTable(unsigned int num_decision_nodes, unsigned int features, unsigned int branch, unsigned int block_size, unsigned int items);
    /**
     * @description: 
     *    Recovery of memory
     */
    virtual ~StrategyTable();

    /**
     * @description:
     *    insert a  value of decision condition with unsigned int and than 
     *    it will create a vector of decision condition into table
     * @param[in] operator (EBIGGER, BIGGER, SMALLER, ESMALLER, EQUAL)
     * @param[in] feature_index  index of features
     * @param[in] value The value of decision condition
     * @return: 
     *    1 is successful otherwise is failed
     */
    virtual int insert_compare_uint(OPERATE operate, unsigned int feature_index, unsigned int value);

    /**
     * @description:
     *    insert a  value of decision condition with float and than 
     *    it will create a vector of decision condition into table
     * @param[in] operator (EBIGGER, BIGGER, SMALLER, ESMALLER, EQUAL)
     * @param[in] feature_index  index of features
     * @param[in] value The value of decision condition
     * @return: 
     *    1 is successful otherwise is failed
     */
    virtual int insert_compare(OPERATE operate, unsigned int feature_index, float value);

    /**
     * @description:
     *    insert a  value of decision condition with int and than 
     *    it will create a vector of decision condition into table
     * @param[in] operator (EBIGGER, BIGGER, SMALLER, ESMALLER, EQUAL)
     * @param[in] feature_index  index of features
     * @param[in] value The value of decision condition
     * @return: 
     *    1 is successful otherwise is failed
     */
    virtual int insert_compare(OPERATE operate, unsigned int feature_index, int value);

    /**
     * @description:
     *     Implementing the set belongs to the decision function. 
     *     And it will create a vector of decision condition into table.
     *     Here will input a bit string, each element is a unsigned char value (0 or 1) that
     *     represents the index of the a set element.
     * @param[in] feature_index  index of features
     * @param[in] value_bits a bit string, such as {0,1,0,1,1,1}
     * @param[in] len the length of value_bits
     * @return: 
     *    1 is successful otherwise is failed
     */
    virtual int insert_in_set(unsigned int feature_index, unsigned char value_bits[], unsigned int len);

    /**
     * @description: 
     *    save the table data into a file
     * @param[in] file_name  the name of file that saves the table data
     * @return: 
     *   1 is successful otherwise is failed
     */
    int save_table(string file_name);

    /**
     * @description: 
     *    load the table data with a file
     * @param[in] file_name  the name of file that loads the table data
     * @return: 
     *   1 is successful otherwise is failed
     */
    int load_table(string file_name);

    /**
     * @description: 
     *    clear the table datda
     * @return: it 
     */
    void clear_table();
};
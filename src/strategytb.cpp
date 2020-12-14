/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-11 20:02:38
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-18 14:39:10
 * @Description: 
 */
#include "HFB/strategytb.h"
#include "HFB/err.h"
#include <stdio.h>
#include <memory.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

const unsigned int MIDDLE_CODE = 0X01UL;
const unsigned int ACCEPT_CODE_TWO = 0X02UL;
const unsigned int ACCEPT_CODE_ONE = 0X01UL;

using namespace std;

void StrategyTable::init(unsigned int num_decision_nodes, unsigned int features, unsigned int branch, unsigned int block_size, unsigned int items)
{
    m_block_index = 0;
    m_branch = branch;
    m_block_size = num_decision_nodes > block_size ? num_decision_nodes : block_size;
    m_items = items;
    m_features = features;
    m_log_items = (unsigned int)(log2f32((float)(m_items)));

    m_table = new unsigned char **[m_branch]; // split block
    for (unsigned int i = 0; i < m_branch; i++)
    {
        m_table[i] = new unsigned char *[m_block_size]; //row
        for (unsigned int j = 0; j < m_block_size; j++)
        {
            m_table[i][j] = new unsigned char[m_items * m_features];
            memset(m_table[i][j], 0, m_items * m_features);
        }
    }
}

StrategyTable::StrategyTable(unsigned int num_decision_nodes, unsigned int features, unsigned int branch, unsigned int block_size, unsigned int items)
{
    init(num_decision_nodes, features, branch, block_size, items);
}

StrategyTable::StrategyTable(unsigned int num_decision_nodes, unsigned int features)
{
    init(num_decision_nodes, features);
}

StrategyTable::~StrategyTable()
{
    if (m_table == NULL)
    {
        return;
    }

    for (unsigned int i = 0; i < m_branch; i++)
    {
        for (unsigned int j = 0; j < m_block_size; j++)
        {
            memset(m_table[i][j], 0, m_items * m_features);
            delete[] m_table[i][j];
            m_table[i][j] = NULL;
        }
        delete[] m_table[i];
        m_table[i] = NULL;
    }
    delete[] m_table;
    m_table = NULL;
    m_branch = 0;
    m_block_size = 0;
    m_items = 0;
    m_log_items = 0;
    m_block_index = 0;
    m_features = 0;
}

int StrategyTable::insert_compare(OPERATE operate, unsigned int feature_index, float value)
{
    int ret;
    unsigned int tmp = float_trans_uint(value);
    ret = insert_compare_uint(operate, feature_index, tmp);
    return ret;
}

int StrategyTable::insert_compare(OPERATE operate, unsigned int feature_index, int value)
{
    int ret;
    unsigned int tmp = int_trans_uint(value);
    ret = insert_compare_uint(operate, feature_index, tmp);
    return ret;
}

int StrategyTable::insert_compare_uint(OPERATE operate, unsigned int feature_index, unsigned int value)
{
    if (m_block_index >= m_block_size || feature_index >= m_features)
    {
        return INSERT_FAILED;
    }

    switch (operate)
    {
    case OPERATE::BIGGER:
        create_vector_bigger(value, feature_index, false);
        break;
    case OPERATE::EBIGGER:
        create_vector_bigger(value, feature_index, true);
        break;
    case OPERATE::SMALLER:
        create_vector_smaller(value, feature_index, false);
        break;
    case OPERATE::ESMALLER:
        create_vector_smaller(value, feature_index, true);
        break;
    case OPERATE::EQUAL:
        create_vector_equal(value, feature_index);
        break;
    default:
        return INSERT_FAILED;
    }
    return SUCCESSFUL;
}

void StrategyTable::create_vector_bigger(unsigned int value, unsigned int feature_index, bool has_equal)
{
    unsigned int i = 0;
    unsigned int per_branch = m_branch - 1;
    unsigned int shift_r = per_branch * m_log_items;
    unsigned int index = feature_index * m_items; // find feature position
    for (; i < per_branch; i++)                   // split table
    {
        unsigned int shift_l = per_branch - i - 1;
        /*branch the data and get each item vaule to create a vector */
        unsigned int item = (unsigned int)((value << (i * m_log_items)) >> shift_r);
        unsigned int j = index + item;
        // item == j
        unsigned int code = MIDDLE_CODE;
        m_table[i][m_block_index][j] = code << shift_l;
        // item < j
        for (j = j + 1; j < index + m_items; j++)
        {
            code = ACCEPT_CODE_TWO;
            m_table[i][m_block_index][j] = code << shift_l;
        }
    }
    /*last table data that only value is 1 or 0 */
    unsigned int item = (unsigned int)((value << (i * m_log_items)) >> shift_r);

    for (unsigned int j = index + item + 1; j < index + m_items; j++)
    {
        m_table[i][m_block_index][j] = ACCEPT_CODE_ONE;
    }
    if (has_equal)
    {
        m_table[i][m_block_index][index + item] = ACCEPT_CODE_ONE;
    }
    m_block_index++;
}

void StrategyTable::create_vector_smaller(unsigned int value, unsigned int feature_index, bool has_equal)
{
    unsigned int i = 0;
    unsigned int per_branch = m_branch - 1;
    unsigned int shift_r = per_branch * m_log_items;
    unsigned int index = feature_index * m_items;
    for (; i < per_branch; i++) // split table
    {
        unsigned int shift_l = per_branch - i - 1;
        /*branch the data and get each item vaule to create a vector */
        unsigned int item = (unsigned int)((value << (i * m_log_items)) >> shift_r);
        unsigned int j = index;
        // item > j
        unsigned int code;
        for (; j < index + item; j++)
        {
            code = ACCEPT_CODE_TWO;
            m_table[i][m_block_index][j] = code << shift_l;
        }
        // item == j
        code = MIDDLE_CODE;
        m_table[i][m_block_index][j] = code << shift_l;
    }
    /*last table data that only value is 1 or 0 */
    unsigned int item = (unsigned int)((value << (i * m_log_items)) >> shift_r);

    for (unsigned int j = index; j < index + item; j++)
    {
        m_table[i][m_block_index][j] = ACCEPT_CODE_ONE;
    }
    if (has_equal)
    {
        m_table[i][m_block_index][index + item] = ACCEPT_CODE_ONE;
    }
    m_block_index++;
}

void StrategyTable::create_vector_equal(unsigned int value, unsigned int feature_index)
{
    unsigned int i;
    unsigned int per_branch = m_branch - 1;
    unsigned int shift_r = per_branch * m_log_items;
    unsigned int index = feature_index * m_items;
    for (i = 0; i < per_branch; i++)
    {
        unsigned int shift_l = per_branch - i - 1;
        /*branch the data and get each item vaule to create a vector */
        unsigned int item = (unsigned int)((value << (i * m_log_items)) >> shift_r);
        unsigned int code = ACCEPT_CODE_TWO;
        m_table[i][m_block_index][index + item] = code << shift_l;
    }
    /*last table data that only value is 1 or 0 */
    unsigned int item = (unsigned int)((value << (i * m_log_items)) >> shift_r);
    m_table[i][m_block_index][index + item] = ACCEPT_CODE_ONE;
    m_block_index++;
}

int StrategyTable::insert_in_set(unsigned int feature_index, unsigned char value_bits[], unsigned int len)
{
    unsigned int shift_l;
    if (m_block_index >= m_block_size || feature_index >= m_features)
    {
        return BLOCK_SIZE_TOO_BIGGER;
    }

    if (len > m_items)
    {
        len = m_items;
    }
    shift_l = m_branch - 1;
    unsigned index = feature_index * m_items;
    for (unsigned int i = index, j = 0; i < index + len; i++, j++)
    {
        unsigned int code = (value_bits[j] & 0x1);
        m_table[0][m_block_index][i] = code << shift_l;
    }
    m_block_index++;

    return SUCCESSFUL;
}

int StrategyTable::save_table(string file_path)
{

    int ret = SUCCESSFUL;
    ofstream out;
    out.open(file_path, ios::out);
    if (!out)
    {
        return OPEN_FILE_ERROR;
    }
    /*save the table size into file */
    out << setw(4) << m_block_index << endl;

    for (unsigned int i = 0; i < m_branch; i++)
    {
        for (unsigned int j = 0; j < m_block_index; j++)
        {
            for (unsigned int k = 0; k < m_items * m_features; k++)
            {
                out << setw(4) << (int)m_table[i][j][k];
            }
            out << endl;
        }
        out << endl;
    }

    out.close();
    return ret;
}

int StrategyTable::load_table(string file_path)
{
    int ret = SUCCESSFUL;
    ifstream in;
    in.open(file_path, ios::in);
    if (!in)
    {
        return OPEN_FILE_ERROR;
    }
    in >> m_block_index; // get table size

    for (unsigned int i = 0; i < m_branch; i++)
    {
        for (unsigned int j = 0; j < m_block_index; j++)
        {
            for (unsigned int k = 0; k < m_items * m_features; k++)
            {
                unsigned int tmp;
                in >> tmp;
                m_table[i][j][k] = (unsigned int)tmp;
            }
        }
    }
    in.close();
    return ret;
}

void StrategyTable::clear_table()
{
    for (unsigned int i = 0; i < m_branch; i++)
    {
        for (unsigned int j = 0; j < m_block_size; j++)
        {
            memset(m_table[i][j], 0, m_items * m_features);
        }
    }
    m_block_index = 0;
}

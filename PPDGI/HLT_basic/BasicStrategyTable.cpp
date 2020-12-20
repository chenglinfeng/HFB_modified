//
// Created by anson on 2020/12/16.
//

#include "HFB/err.h"
#include <stdio.h>
#include <memory.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "basicstrategytable.h"

using namespace std;

int BIT_LEN = 12;

void BasicStrategyTable::init(unsigned int num_decision_nodes, unsigned int features, unsigned int block_size){

    m_block_index = 0;
    m_block_size = num_decision_nodes > block_size ? num_decision_nodes : block_size;
    m_features = features;

    m_table = new unsigned char *[m_block_size];
    for(unsigned int i = 0; i < m_block_size; i++){
        long long size = m_features * pow(2,BIT_LEN);
        m_table[i] = new unsigned char [size];
        memset(m_table[i], 0, size);
    }
}

BasicStrategyTable::BasicStrategyTable(unsigned int num_decision_nodes, unsigned int features){
    init(num_decision_nodes,features);
}

 BasicStrategyTable::~BasicStrategyTable(){

    if(m_table == NULL){
        return;
    }

    for(unsigned int i = 0; i < m_block_size; i++){
         long long size = m_features * pow(2,BIT_LEN);
         memset(m_table[i], 0, size);
         delete [] m_table[i];
         m_table[i] = NULL;
    }
    delete [] m_table;
    m_table = NULL;
    m_block_size = 0;
    m_block_index = 0;
    m_features = 0;
}

int BasicStrategyTable::insert_compare_uint(OPERATE operate, unsigned int feature_index, unsigned int value){

    if (m_block_index >= m_block_size || feature_index >= m_features)
    {
        return INSERT_FAILED;
    }

    switch (operate)
    {

        case OPERATE::SMALLER:
            create_vector_smaller(value, feature_index, false);
            break;
        case OPERATE::ESMALLER:
            create_vector_smaller(value, feature_index, true);
            break;
        default:
            return INSERT_FAILED;
    }
    return SUCCESSFUL;
}

void BasicStrategyTable::create_vector_smaller(unsigned int value, unsigned int feature_index, bool has_equal){
    unsigned int tmp  = value << (sizeof(unsigned int) - BIT_LEN) >> (sizeof(unsigned int) - BIT_LEN);
    unsigned int thres = has_equal ? value + 1 : value;
    long long index = feature_index * pow(2,BIT_LEN);
    for(long long i=index; i < index + thres; i++){
        m_table[m_block_index][i] = 1;
    }

    m_block_index++;
}


int BasicStrategyTable::save_table(string file_name){
    int ret = SUCCESSFUL;
    ofstream out;
    out.open(file_name, ios::out);
    if (!out)
    {
        return OPEN_FILE_ERROR;
    }
    for(int i = 0; i < m_block_index; i++){
        for (long long j = 0; j < m_features * pow(2, BIT_LEN); j++){
            out << setw(4) << (int)m_table[i][j];
        }
        out << endl;
    }

    out.close();
    return ret;
}
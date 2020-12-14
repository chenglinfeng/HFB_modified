/*
 * @Author: xin.xing.lzj
 * @Date: 2019-08-12 14:14:49
 * @LastEditors: xinxing.lzj
 * @LastEditTime: 2019-08-21 19:43:45
 * @Description: 
 */


%module treePredict

%{
    #include "treeuser.h"
    #include "treemodel.h"
    #include "treeofflineOT.h"
%}
%include "carrays.i"
%include "cdata.i"
%array_class(unsigned char, bytesArray);
%array_class(unsigned int, uintsArray);


%include "treeuser.h"
%include "treemodel.h"
%include "treeofflineOT.h"

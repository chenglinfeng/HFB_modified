//
// Created by anson on 2020/12/16.
//

#ifndef HFB_HLT_BASIC_ELGAMAL_H
#define HFB_HLT_BASIC_ELGAMAL_H

#include <mcl/fp.hpp>
#include <mcl/ec.hpp>
#include <mcl/elgamal.hpp>
#include <mcl/ecparam.hpp>

typedef mcl::FpT<> Fp;
typedef mcl::FpT<mcl::ZnTag> Zn; // use ZnTag because Zn is different class with Fp
typedef mcl::EcT<Fp> Ec;
typedef mcl::ElgamalT<Ec, Zn> Elgamal;

#endif //HFB_HLT_BASIC_ELGAMAL_H

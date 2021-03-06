/*
    (C) Copyright 2019 CEA LIST. All Rights Reserved.
    Contributor(s): Cingulata team

    This software is governed by the CeCILL-C license under French law and
    abiding by the rules of distribution of free software.  You can  use,
    modify and/ or redistribute the software under the terms of the CeCILL-C
    license as circulated by CEA, CNRS and INRIA at the following URL
    "http://www.cecill.info".

    As a counterpart to the access to the source code and  rights to copy,
    modify and redistribute granted by the license, users are provided only
    with a limited warranty  and the software's author,  the holder of the
    economic rights,  and the successive licensors  have only  limited
    liability.

    The fact that you are presently reading this means that you have had
    knowledge of the CeCILL-C license and that you accept its terms.
*/

#include <ci_bit_vector.hxx>
#include <iostream>
#include <cassert>

using namespace std;
using namespace cingulata;


/**
 * Define name manager format to use for @c CiBitVector types
 */
template<>
string IoNameVec<CiBitVector>::m_name_fmt{ "%s_%d" };

CiBitVector::CiBitVector(
    const int p_bit_cnt,
    const CiBit& p_bit)
:
  m_vec(p_bit_cnt, p_bit)
{}

CiBitVector::CiBitVector(const vector<bit_plain_t>& p_bits)
:
  m_vec(p_bits.begin(), p_bits.end())
{}

CiBitVector::CiBitVector(const vector<CiBit>& p_bits)
:
  m_vec(p_bits)
{}

CiBitVector::CiBitVector(const Slice<CiBitVector>& slice) {
  for (int i = 0; i < (int)slice.size(); ++i) {
    m_vec.push_back(slice[i]);
  }
}

CiBitVector::CiBitVector(const CSlice<CiBitVector>& slice) {
  for (int i = 0; i < (int)slice.size(); ++i) {
    m_vec.push_back(slice[i]);
  }
}

CiBitVector& CiBitVector::operator= (const CiBitVector& other) {
  if (this != &other) {
    m_vec = other.m_vec;
  }
  return *this;
}

unsigned CiBitVector::size() const {
  return m_vec.size();
}

CiBit CiBitVector::multvect() const {
  /* log depth tree */
  CiBitVector tmp(m_vec);
  int size=m_vec.size();
  for (int k = 1; k < size; k *= 2) {
    for (int i = 0; i < size - k; i += 2*k) {
      tmp[i] &= tmp[i+k];
    }
  }

  return tmp[0];
}

CiBitVector& CiBitVector::resize(const unsigned p_bit_cnt, const CiBit& p_bit) {
  m_vec.resize(p_bit_cnt, p_bit);
  return *this;
}

CiBitVector& CiBitVector::append(const CiBit& p_bit) {
  m_vec.push_back(p_bit);
  return *this;
}

CiBit& CiBitVector::operator[](const int p_idx) {
  return at(p_idx);
}

const CiBit& CiBitVector::operator[](const int p_idx) const {
  return at(p_idx, CiBit::zero);
}

CiBit& CiBitVector::at(const int p_idx) {
  unsigned idx = idx_rel_to_abs(p_idx);
  return m_vec.at(idx);
}

const CiBit& CiBitVector::at(const int p_idx, const CiBit& p_bit) const {
  unsigned idx = idx_rel_to_abs(p_idx);
  if (idx < size())
    return m_vec.at(idx);
  else
    return p_bit;
}

Slice<CiBitVector> CiBitVector::slice(const optional<int> &start,
                                      const optional<int> &end,
                                      const optional<int> &stride) {
  return Slice<CiBitVector>(*this, start, end, stride);
}

const CSlice<CiBitVector> CiBitVector::slice(const optional<int> &start,
                                            const optional<int> &end,
                                            const optional<int> &stride) const {
  return CSlice<CiBitVector>(*this, start, end, stride);
}

Slice<CiBitVector> CiBitVector::
operator[](const tuple<optional<int>, optional<int>, optional<int>> &idx) {
  return slice(get<0>(idx), get<1>(idx), get<2>(idx));
}

const CSlice<CiBitVector> CiBitVector::operator[](
    const tuple<optional<int>, optional<int>, optional<int>> &idx) const {
  return slice(get<0>(idx), get<1>(idx), get<2>(idx));
}

CiBitVector& CiBitVector::operator&=(const CiBitVector& other) {
  this->op_and(other, CiBit::one);
  return *this;
}

CiBitVector& CiBitVector::operator|=(const CiBitVector& other) {
  this->op_or(other, CiBit::zero);
  return *this;
}

CiBitVector& CiBitVector::operator^=(const CiBitVector& other) {
  this->op_xor(other, CiBit::zero);
  return *this;
}

CiBitVector& CiBitVector::operator&=(const CiBit& p_bit) {
  this->op_and(p_bit);
  return *this;
}

CiBitVector& CiBitVector::operator|=(const CiBit& p_bit) {
  this->op_or(p_bit);
  return *this;
}

CiBitVector& CiBitVector::operator^=(const CiBit& p_bit) {
  this->op_xor(p_bit);
  return *this;
}

CiBitVector& CiBitVector::op_not() {
  for (unsigned i = 0; i < size(); ++i) {
    (*this)[i].op_not();
  }
  return *this;
}

#define DEFINE_BITWISE_MEMBER_FUNC_BV(OP_NAME, SAME_OPERANDS_CODE) \
CiBitVector& CiBitVector::OP_NAME(const CiBitVector& other, const CiBit& p_bit) { \
  if (this == &other) { \
    SAME_OPERANDS_CODE; \
  } else { \
    unsigned n = min(size(), other.size()); \
    for (unsigned i = 0; i < n; ++i) { \
      (*this)[i].OP_NAME(other[i]); \
    } \
    for (unsigned i = n; i < size(); ++i) { \
      (*this)[i].OP_NAME(p_bit); \
    } \
  } \
  return *this; \
}

DEFINE_BITWISE_MEMBER_FUNC_BV(op_and  , );
DEFINE_BITWISE_MEMBER_FUNC_BV(op_nand , op_not());
DEFINE_BITWISE_MEMBER_FUNC_BV(op_andny, m_vec.assign(size(), CiBit::zero));
DEFINE_BITWISE_MEMBER_FUNC_BV(op_andyn, m_vec.assign(size(), CiBit::zero));
DEFINE_BITWISE_MEMBER_FUNC_BV(op_or   , );
DEFINE_BITWISE_MEMBER_FUNC_BV(op_nor  , op_not());
DEFINE_BITWISE_MEMBER_FUNC_BV(op_orny , m_vec.assign(size(), CiBit::one));
DEFINE_BITWISE_MEMBER_FUNC_BV(op_oryn , m_vec.assign(size(), CiBit::one));
DEFINE_BITWISE_MEMBER_FUNC_BV(op_xor  , m_vec.assign(size(), CiBit::zero));
DEFINE_BITWISE_MEMBER_FUNC_BV(op_xnor , m_vec.assign(size(), CiBit::one));

#define DEFINE_BITWISE_MEMBER_FUNC_B(OP_NAME) \
CiBitVector& CiBitVector::OP_NAME(const CiBit& p_bit) { \
  for (unsigned i = 0; i < size(); ++i) { \
    (*this)[i].OP_NAME(p_bit); \
  } \
  return *this; \
}

DEFINE_BITWISE_MEMBER_FUNC_B(op_and);
DEFINE_BITWISE_MEMBER_FUNC_B(op_nand);
DEFINE_BITWISE_MEMBER_FUNC_B(op_andny);
DEFINE_BITWISE_MEMBER_FUNC_B(op_andyn);
DEFINE_BITWISE_MEMBER_FUNC_B(op_or);
DEFINE_BITWISE_MEMBER_FUNC_B(op_nor);
DEFINE_BITWISE_MEMBER_FUNC_B(op_orny);
DEFINE_BITWISE_MEMBER_FUNC_B(op_oryn);
DEFINE_BITWISE_MEMBER_FUNC_B(op_xor);
DEFINE_BITWISE_MEMBER_FUNC_B(op_xnor);


CiBitVector& CiBitVector::shl(const int pos, const CiBit& p_bit) {
  if (size() == 0)
    return *this;

  if (pos < 0)
    return this->shr(-pos, p_bit);

  if (pos > 0) {
    unsigned ppos = (pos > (int)size()) ? size() : pos;
    m_vec.resize(size() + ppos, p_bit);
    m_vec.erase(m_vec.begin(), m_vec.begin() + ppos);
  }

  return *this;
}

CiBitVector& CiBitVector::shr(const int pos, const CiBit& p_bit) {
  if (size() == 0)
    return *this;

  if (pos < 0)
    return this->shl(-pos, p_bit);

  if (pos > 0) {
    unsigned ppos = (pos > (int)size()) ? size() : pos;
    m_vec.resize(size() - ppos);
    m_vec.insert(m_vec.begin(), ppos, p_bit);
  }

  return *this;
}

CiBitVector& CiBitVector::rol(const int pos) {
  if (size() > 0 and pos != 0) {
    unsigned ppos = (pos > 0) ? (pos % size()) : (size() - (-pos % size()));

    auto bits_cpy = m_vec;
    m_vec = vector<CiBit>(bits_cpy.begin()+ppos, bits_cpy.end());
    m_vec.insert(m_vec.end(), bits_cpy.begin(), bits_cpy.begin()+ppos);
  }

  return *this;
}

CiBitVector& CiBitVector::ror(const int pos) {
  return rol(-pos);
}

CiBitVector& CiBitVector::operator<<= (const int pos) {
  return shl(pos, CiBit::zero);
}

CiBitVector& CiBitVector::operator>>= (const int pos) {
  return shr(pos, CiBit::zero);
}


int CiBitVector::idx_rel_to_abs(int idx) const {
  if (idx < 0)
    idx += size();
  return idx;
}

unsigned CiBitVector::idx_clip(int idx) const {
  if (idx < 0)
    idx = 0;
  else if (idx > (int)size())
    idx = size();
  return idx;
}

CiBitVector cingulata::operator~(CiBitVector lhs) {
  return lhs.op_not();
}

CiBitVector cingulata::operator^(CiBitVector lhs, const CiBitVector& rhs) {
  return lhs ^= rhs;
}

CiBitVector cingulata::operator&(CiBitVector lhs, const CiBitVector& rhs) {
  return lhs &= rhs;
}

CiBitVector cingulata::operator|(CiBitVector lhs, const CiBitVector& rhs) {
  return lhs |= rhs;
}

CiBitVector cingulata::shl(CiBitVector lhs, const int pos, const CiBit& p_bit) {
  return lhs.shl(pos, p_bit);
}

CiBitVector cingulata::shr(CiBitVector lhs, const int pos, const CiBit& p_bit) {
  return lhs.shr(pos, p_bit);
}

CiBitVector cingulata::rol(CiBitVector lhs, const int pos) {
  return lhs.rol(pos);
}

CiBitVector cingulata::ror(CiBitVector lhs, const int pos) {
  return lhs.ror(pos);
}

CiBitVector cingulata::operator<< (CiBitVector lhs, const int pos) {
  return lhs <<= pos;
}

CiBitVector cingulata::operator>> (CiBitVector lhs, const int pos) {
  return lhs >>= pos;
}

istream& cingulata::operator>>(istream& inp, CiBitVector& val) {
  val.read();
  return inp;
}

ostream& cingulata::operator<<(ostream& out, const CiBitVector& val) {
  val.write();
  return out;
}


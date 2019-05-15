// Concord
//
// Copyright (c) 2018 VMware, Inc. All Rights Reserved.
//
// This product is licensed to you under the Apache 2.0 license (the "License").
// You may not use this product except in compliance with the Apache 2.0
// License.
//
// This product may include a number of subcomponents with separate copyright
// notices and license terms. Your use of these subcomponents is subject to the
// terms and conditions of the subcomponent's license, as noted in the LICENSE
// file.

#pragma once

#include "PrimitiveTypes.hpp"
#include "assertUtils.hpp"

#include <vector>
#include <cstring>

namespace bftEngine {
namespace impl {
class Bitmap {
 public:
  Bitmap() : numBits_(0), p_(nullptr) {}

  Bitmap(uint32_t numBits) : numBits_(numBits), p_(nullptr) {
    if (numBits_ > 0) {
      p_ = (unsigned char*)std::malloc(realSize());
      zeroAll();
    }
  }

  Bitmap(const Bitmap& other) : numBits_(other.numBits_), p_(nullptr) {
    if (numBits_ == 0) {
      p_ = nullptr;
    } else {
      p_ = (unsigned char*)std::malloc(realSize());
      std::memcpy(p_, other.p_, realSize());
    }
  }

  ~Bitmap() {
    if (numBits_ > 0) {
      Assert(p_ != nullptr);
      std::free(p_);
    }
  }

  Bitmap& operator=(const Bitmap& other) {
    if (numBits_ > 0) {
      Assert(p_ != nullptr);
      std::free(p_);
    }

    numBits_ = other.numBits_;
    if (numBits_ == 0) {
      p_ = nullptr;
    } else {
      p_ = (unsigned char*)std::malloc(realSize());
      std::memcpy(p_, other.p_, realSize());
    }

    return *this;
  }

  void zeroAll() {
    Assert(p_ != nullptr);
    memset((void*)p_, 0, realSize());
  }

	uint32_t numOfBits() const { return numBits_; }

  bool get(uint32_t i) const {
    Assert(i < numBits_);
    const uint32_t byteIndex = i / 8;
    const unsigned char byteMask = (1 << (i % 8));
    return ((p_[byteIndex] & byteMask) != 0);
  }

  void set(uint32_t i) {
    Assert(i < numBits_);
    const uint32_t byteIndex = i / 8;
    const unsigned char byteMask = (1 << (i % 8));
    p_[byteIndex] = p_[byteIndex] | byteMask;
  }

  void reset(uint32_t i) {
    Assert(i < numBits_);
    const uint32_t byteIndex = i / 8;
    const unsigned char byteMask = ((unsigned char)0xFF) & ~(1 << (i % 8));
    p_[byteIndex] = p_[byteIndex] & byteMask;
  }

	void writeToBuffer(char* buffer, uint32_t bufferLength, uint32_t* actualSize) const
	{
		const uint32_t sizeNeeded = sizeNeededInBuffer();
		Assert(bufferLength >= sizeNeeded);
		uint32_t* pNumOfBits = (uint32_t*)buffer;
		char* pBitmap = buffer + sizeof(uint32_t);
		*pNumOfBits = numBits_;
		memcpy(pBitmap, p_, realSize());
		if (actualSize) *actualSize = sizeNeeded;
	}

	uint32_t sizeNeededInBuffer() const
	{
		return (sizeof(uint32_t) + realSize(numBits_));
	}

	static Bitmap* createBitmapFromBuffer(char* buffer, uint32_t bufferLength, uint32_t* actualSize)
	{
		Assert(bufferLength >= sizeof(uint32_t));
		uint32_t* pNumOfBits = (uint32_t*)buffer;
		if (*pNumOfBits == 0)
		{
			if(actualSize) *actualSize = sizeof(uint32_t);
			return new Bitmap();
		}
		else
		{
			char* pBitmap = buffer + sizeof(uint32_t);
			Bitmap* b = new Bitmap();
			b->numBits_ = *pNumOfBits;
			b->p_ = (unsigned char*)std::malloc(*pNumOfBits);
			std::memcpy(b->p_, pBitmap, realSize(*pNumOfBits));
			if (actualSize) *actualSize = sizeof(uint32_t) + realSize(*pNumOfBits);
			return b;
		}
	}

	static uint32_t maxSizeNeededToStoreInBuffer(uint32_t maxNumOfBits)
	{
		return (sizeof(uint32_t) + realSize(maxNumOfBits));
	}

 protected:
	uint32_t numBits_;
  unsigned char* p_;

	uint32_t realSize() const { return realSize(numBits_); }

	static uint32_t realSize(uint32_t nbits) { return ((nbits + 7) / 8); }
};
}  // namespace impl
}  // namespace bftEngine

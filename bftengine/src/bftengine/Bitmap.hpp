// Concord
//
// Copyright (c) 2018 VMware, Inc. All Rights Reserved.
//
// This product is licensed to you under the Apache 2.0 license (the "License").
// You may not use this product except in compliance with the Apache 2.0 License.
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


namespace bftEngine
{
namespace impl
{
 	class Bitmap
	{
	public:

		Bitmap() :
			numBits_(0),
			p_(nullptr)
		{
		}

		Bitmap(size_t numBits) :
			numBits_(numBits),
			p_(nullptr)
		{
			if (numBits_ > 0) {
				p_ = (unsigned char*)std::malloc(realSize());
				zeroAll();
			}
		}

		Bitmap(const Bitmap& other) :
			numBits_(other.numBits_),
			p_(nullptr)
		{
			if (numBits_ == 0) {
				p_ = nullptr;
			}
			else {
				p_ = (unsigned char*)std::malloc(realSize());
				std::memcpy(p_, other.p_, realSize());
			}
		}

		~Bitmap()
		{
			if (numBits_ > 0) {
				Assert(p_ != nullptr);
				std::free(p_);
			}
		}

		Bitmap& operator= (const Bitmap& other)
		{
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

		void zeroAll()
		{
			Assert(p_ != nullptr);
			memset((void*)p_, 0, realSize());
		}

		size_t numOfBits() const
		{
			return numBits_;
		}

		bool get(size_t i) const
		{
			Assert(i < numBits_);
			const size_t byteIndex = i / 8;
			const unsigned char byteMask = (1 << (i % 8));
			return ((p_[byteIndex] & byteMask) != 0);
		}

		void set(size_t i)
		{
			Assert(i < numBits_);
			const size_t byteIndex = i / 8;
			const unsigned char byteMask = (1 << (i % 8));
			p_[byteIndex] = p_[byteIndex] | byteMask;
		}

		void reset(size_t i)
		{
			Assert(i < numBits_);
			const size_t byteIndex = i / 8;
			const unsigned char byteMask = ((unsigned char)0xFF) & ~(1 << (i % 8));
			p_[byteIndex] = p_[byteIndex] & byteMask;
		}

	protected:

		size_t numBits_;
		unsigned char* p_;

		size_t realSize() const
		{
			return ((numBits_ + 7) / 8);
		}

	};
}
}

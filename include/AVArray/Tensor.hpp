#include "AVArray.hpp"

#ifndef TENSOR_HPP
#define TENSOR_HPP

template<typename T, unsigned int D>
class Tensor : public AVArray<T, D>
{
	typedef Tensor<T, D - 1> ElementType;
	typedef std::array<unsigned int, D> Shape;

public:
	Tensor(const AVArray<T, D> &other) : AVArray<T, D>(other) {}
	Tensor(AVArray<T, D> &&other) : AVArray<T, D>(std::move(other)) {}

public:
	template<unsigned int Din,
	typename std::enable_if<(D - Din + 2 > 1)>::type * = nullptr>
	void multHelper(const Tensor<T, D - Din + 2> &lhs, const Tensor<T, Din> &rhs)
	{
		for (unsigned int i = 0; i < this->dims[0]; ++i)
		{
			(*this)[i].multHelper(lhs[i], rhs);
		}
		return;
	}

	template<unsigned int Din>
	void multHelper(const Tensor<T, 1> &lhs, const Tensor<T, Din> &rhs)
	{
		assert(lhs.dims[0] == rhs.dims[0]);
		for (int i = 0; i < this->dims[0]; ++i)
		{
			*this += lhs[i]*rhs[i];
		}
	}

public:
	inline ElementType operator[](unsigned int i)
	{
		return AVArray<T, D>::operator[](i);
	}

	inline const ElementType operator[](unsigned int i) const
	{
		return AVArray<T, D>::operator[](i);
	}

	using AVArray<T, D>::AVArray;
	// Tensor() : AVArray<T, D>() {}

	// template<typename ...Ints>
	// Tensor(unsigned int n1, Ints... nD) : AVArray<T, D>(n1, nD...) {}

	// Tensor(Shape new_dims) : AVArray<T, D>(new_dims) {}

	// Tensor(const Tensor &other) = default;
	// Tensor(Tensor &&other) = default;
	// Tensor& operator=(const Tensor &other) = default;
	// Tensor& operator=(Tensor &&other) = default;
	virtual ~Tensor() = default;

	template<unsigned int Din>
	Tensor<T, D + Din - 2> operator*(const Tensor<T, Din> &other) const
	{
		typedef std::array<unsigned int, D + Din - 2> ShapeOut;
		ShapeOut shape_out;
		for (unsigned int i = 0; i < D - 1; ++i)
		{
			shape_out[i] = this->dims[i];
		}
		for (unsigned int i = 1; i < Din; ++i)
		{
			shape_out[D + i - 2] = other.dims[i];
		}
		Tensor<T, D + Din - 2> tensor_out(shape_out);
		tensor_out.setZero();
		for (unsigned int i = 0; i < this->dims[0]; ++i)
		{
			tensor_out[i].multHelper((*this)[i], other);
		}
		return tensor_out;
	}

	void setZero()
	{
		for (unsigned int i = 0; i < this->getSize(this->dims); ++i)
		{
			this->storage[i] = 0.0;
		}
		return;
	}

	Tensor& operator*=(const T constant)
	{
		for (unsigned int i = 0; i < this->getSize(this->dims); ++i)
		{
			this->storage[i] *= constant;
		}
		return *this;
	}

	friend Tensor operator*(const T lhs, const Tensor &rhs)
	{
		Tensor out(rhs);
		out *= lhs;
		return out;
	}

	friend Tensor operator*(const Tensor &lhs, const T rhs)
	{
		Tensor out(lhs);
		out *= rhs;
		return out;
	}

	Tensor& operator+=(const Tensor &other)
	{
		assert(this->dims == other.dims);
		for (unsigned int i = 0; i < this->getSize(this->dims); ++i)
		{
			this->storage[i] += other.storage[i];
		}
		return *this;
	}

	friend Tensor operator+(Tensor lhs, const Tensor &rhs)
	{
		lhs += rhs;
		return lhs;
	}
};

template<typename T>
class Tensor<T, 1> : public AVArray<T, 1>
{
	typedef T& ElementType;
	typedef const T& ConstElementType;
	typedef std::array<unsigned int, 1> Shape;

public:
	Tensor(const AVArray<T, 1> &other) : AVArray<T, 1>(other) {}
	Tensor(AVArray<T, 1> &&other) : AVArray<T, 1>(std::move(other)) {}

public:
	template<unsigned int Din>
	void multHelper(const Tensor<T, 1> &lhs, const Tensor<T, Din> &rhs)
	{
		for (unsigned int i = 0; i < this->dims[0]; ++i)
		{
			*this += lhs[i]*rhs[i];
		}
	}

public:
	inline ElementType operator[](unsigned int i)
	{
		return AVArray<T, 1>::operator[](i);
	}

	inline ConstElementType operator[](unsigned int i) const
	{
		return AVArray<T, 1>::operator[](i);
	}

	using AVArray<T, 1>::AVArray;

	// Tensor() : AVArray<T, 1>() {}

	// Tensor(unsigned int n1) : AVArray<T, 1>(n1) {}

	// Tensor(Shape new_dims) : AVArray<T, 1>(new_dims) {}

	// Tensor(const Tensor &other) = default;
	// Tensor(Tensor &&other) = default;
	// Tensor& operator=(const Tensor &other) = default;
	// Tensor& operator=(Tensor &&other) = default;
	virtual ~Tensor() = default;
	
	template<unsigned int Din>
	Tensor<T, Din - 1> operator*(const Tensor<T, Din> &other) const
	{
		typedef std::array<unsigned int, Din - 1> ShapeOut;
		ShapeOut shape_out(other.makeSubshape(other.dims));
		Tensor<T, Din - 1> tensor_out(shape_out);
		tensor_out.setZero();
		tensor_out.multHelper(*this, other);
		return tensor_out;
	}

	void setZero()
	{
		for (unsigned int i = 0; i < this->dims[0]; ++i)
		{
			this->storage[i] = 0.0;
		}
		return;
	}

	Tensor& operator*=(const T constant)
	{
		for (unsigned int i = 0; i < this->dims[0]; ++i)
		{
			this->storage[i] *= constant;
		}
		return *this;
	}

	friend Tensor operator*(const T lhs, const Tensor &rhs)
	{
		Tensor out(rhs);
		out *= lhs;
		return out;
	}

	friend Tensor operator*(const Tensor &lhs, const T rhs)
	{
		Tensor out(lhs);
		out *= rhs;
		return out;
	}

	Tensor& operator+=(const Tensor &other)
	{
		assert(this->dims == other.dims);
		for (unsigned int i = 0; i < this->dims[0]; ++i)
		{
			this->storage[i] += other.storage[i];
		}
		return *this;
	}

	friend Tensor operator+(Tensor lhs, const Tensor &rhs)
	{
		lhs += rhs;
		return lhs;
	}
};

#endif // TENSOR_HPP
#ifndef TENSOR_BASE_HPP
#define TENSOR_BASE_HPP

template<typename Derived, int D>
class TensorBase {
public:
	template<typename T>
	using element_type = typename Derived<T, D>::element_type;

private:
	Derived& derived() { return static_cast<Derived&>(*this); }

};

#endif // TENSOR_BASE_HPP

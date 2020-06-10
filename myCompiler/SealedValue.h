#pragma once
struct Sealed
{
	void* ptr = nullptr;
	virtual ~Sealed() {}
};

template<typename T>
struct SealedValue :public Sealed
{
	using Type = T;
	SealedValue(T val) :value(new T(val))
	{
		ptr = value;
	}
	SealedValue(T* obj_ptr) :value(obj_ptr)
	{
		ptr = obj_ptr;
	}
	T* value;
	virtual ~SealedValue()
	{
		delete value;
	}
};

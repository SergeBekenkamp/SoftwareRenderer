#pragma once

template <class T>
class linkedList {
private:
	linkedList<T> next;
	linkedList<T> prev;
public:
	T current;

	linkedList<T>(T val) {
		current = val;
	}
	

	linkedList<T> linkedList<T>::getNext() {
		return next;
	}
	linkedList<T> linkedList<T>::getPrev() {
		return prev;
	}
	void linkedList<T>::Add(T val) {
		if (next == nullptr) {
			next = new linkedList<T>(val);
		}
		else {
			next->Add(val);
		}
	}
	
};
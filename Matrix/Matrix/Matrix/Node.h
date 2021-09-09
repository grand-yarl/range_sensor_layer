#pragma once
using namespace std;
template <typename T>
class Node
{
	private:
		T data;
		Node* prev;
		Node* next;
	public:
		Node(T data = 0);

		T get_data() const;
		void set_data(T value);
		Node* get_prev() const;

		Node* get_next() const;

		~Node();
};

template <typename T>
Node<T>::Node(T data)
{
	this->data = data;
	cout << "construct" << '\n';
}

template <typename T>
T Node<T>::get_data() const
{
	return this->data;
}

template <typename T>
void Node<T>::set_data(T value)
{
	this->data = value;
	return;
}

template <typename T>
Node<T>* Node<T>::get_prev() const
{
	return this->prev;
}




template <typename T>
Node<T>* Node<T>::get_next() const
{
	return this->next;
}

template <typename T>
Node<T>::~Node()
{
	delete prev;
	delete next;
	cout << "destruct" << '\n';
}




class que
{
	private:

};
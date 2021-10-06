#pragma once
using namespace std;
template <typename T>
class Node
{
	public:
		T data;
		Node* prev = nullptr;
		Node* next = nullptr;
		Node(T d = NULL);
		Node(const Node& cpy);
		Node& new_data(T d);
		T get_data();
		Node& operator=(const Node& cpy);
		~Node();
};

template <typename T>
Node<T>::Node(T d)
{
	this->data = d;
	cout << "construct new Node" << '\n';
}

template <typename T>
Node<T>::Node<T>(const Node<T>& cpy)
{
	if (&cpy == nullptr)
	{
		cout << "There is no node to construct" << '\n';
		system("pause");
		exit(1);
	}
	this->data = cpy.data;
	this->prev = cpy.prev;
	this->next = cpy.next;
}

template <typename T>
Node<T>& Node<T>::new_data(T d)
{
	this->data = d;
	return *this;
}

template <typename T>
T Node<T>::get_data()
{
	return this->data;
}

template <typename T>
Node<T>& Node<T>::operator=(const Node<T>& cpy)
{
	this->data = cpy.data;
	this->prev = cpy.prev;
	this->next = cpy.next;
	return *this;
}

template <typename T>
Node<T>::~Node()
{
	delete prev;
	delete next;
	cout << "destruct Node" << '\n';
}



template <typename T>
class que
{
	private:
		Node<T>* beg;
		Node<T>* end;
		int len = 0;
	public:
		que();
		que(Node<T>& first);
		que(const que<T>& cpy);
		que& operator=(const que& cpy);
		void push(Node<T>& nw);
		Node<T>* get();
		Node<T>* del();
		int lenght() const;
		Node<T>* search(T value);
		friend ostream& operator<<(ostream &os, const que<T> &c)
		{
			if (c.len == 0)
			{
				os << "que is empty" << '\n';
				return os;
			}
			else
			{
				Node<T>* current = c.beg;
				while (current)
				{
					os << "->" << current->get_data();
					current = current->next;
				}
				return os;
			}
		}
		~que();

};
template <typename T>
que<T>::que()
{
	this->beg = 0;
	this->end = 0;
	this->len = 0;
	cout << "construct new que" << '\n';
}

template <typename T>
que<T>::que(Node<T>& first)
{
	this->beg = &first;
	this->end = &first;
	this->len = 1;
	cout << "construct new que" << '\n';
}

template <typename T>
que<T>::que(const que<T>& cpy)
{
	Node<T>* current = cpy.beg;
	this->len = 0;
	Node<T>* input = new Node<T>[cpy.lenght()];
	for (int i = 0; i < cpy.lenght(); i++)
	{
		input[i].new_data(current->get_data());
		this->push(input[i]);
		cout << input[i].data << '\n';
		current = current->next;
	}
	cout << "construct new que" << '\n';
}

template <typename T>
que<T>& que<T>::operator=(const que<T>& cpy)
{
	while (this->len != 0)
	{
		this->del();
	}
	Node<T>* current = cpy.beg;
	this->len = 0;
	Node<T>* input = new Node<T>[cpy.lenght()];
	for (int i = 0; i < cpy.lenght(); i++)
	{
		input[i].new_data(current->get_data());
		this->push(input[i]);
		cout << input[i].data << '\n';
		current = current->next;
	}
	return *this;
}

template <typename T>
void que<T>::push(Node<T>& nw)
{
	if (len == 0)
	{
		this->beg = &nw;
		this->end = &nw;
		this->len = 1;
		cout << "element is pushed" << '\n';
		return;
	}
	this->end->next = &nw;
	nw.prev = this->end;
	this->end = &nw;
	this->len += 1;
	cout << "element is pushed" << '\n';
}

template <typename T>
Node<T>* que<T>::get()
{
	if (this->beg == nullptr)
	{
		cout << "there is no element in que" << '\n';
		return 0;
	}
	if (this->len == 1)
	{
		Node<T>* temp = this->beg;
		this->beg = 0;
		this->end = 0;
		this->len -= 1;
		cout << "element is out" << '\n';
		return temp;
	}
	Node<T>* temp = this->beg;
	this->beg = this->beg->next;
	this->beg->prev = 0;
	temp->next = 0;
	this->len -= 1;
	cout << "element is out" << '\n';
	return temp;
}

template <typename T>
Node<T>* que<T>::del()
{
	if (this->beg == nullptr)
	{
		cout << "there is no element in que" << '\n';
		return 0;
	}
	if (this->len == 1)
	{
		Node<T>* temp = this->beg;
		this->beg = 0;
		this->end = 0;
		this->len -= 1;
		cout << "element deleted" << '\n';
		return temp;
	}
	Node<T>* temp = this->beg;
	this->beg = this->beg->next;
	this->beg->prev = 0;
	temp->next = 0;
	this->len -= 1;
	cout << "element is deleted" << '\n';
	return this->beg;
}

template <typename T>
int que<T>::lenght() const
{
	return this->len;
}

template <typename T>
Node<T>* que<T>::search(T value)
{
	if (this->len == 0)
	{
		cout << "there is no element in que" << '\n';
		return nullptr;
	}
	Node<T>* current = this->beg;
	while (current)
	{
		if (current->data == value)
		{
			cout << "element is found" << '\n';
			return current;
		}
		else
			current = current->next;
	}
	cout << "element is not found" << '\n';
	return nullptr;

}

template <typename T>
que<T>::~que()
{
	while (this->len != 0)
	{
		this->del();
		cout << this->len << '\n';
	}
	delete this->beg;
	delete this->end;
	cout << "destruct que" << '\n';
}
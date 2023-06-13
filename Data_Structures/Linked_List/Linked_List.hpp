#ifndef EXT_LINKED_LIST_HPP
#define EXT_LINKED_LIST_HPP

#include "../Vector/Vector.hpp"

/**
 *     class Iterator {
    private:
        Node* current;

    public:
        Iterator(Node* ptr) : current(ptr) {}

        T& operator*() const { return current->value; }
        T* operator->() const { return &current->value; }

        Iterator& operator++() { current = current->next; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const Iterator& a, const Iterator& b) { return a.current == b.current; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.current != b.current; };
    };
 */

namespace ext {
	template<class T>
	class linked_list {
	public:
		using value_type = T;
		using reference = T &;
		using const_reference = T const &;
		using pointer = T *;
		using const_pointer = T const *;
		using iterator = T *;
		using const_iterator = T const *;
		using riterator = std::reverse_iterator<iterator>;
		using const_riterator = std::reverse_iterator<const_iterator>;
		using difference_type = std::ptrdiff_t;
		using size_type = size_t;

	private:
		class node {
		public:
			T data;
			node *next;

		public:
			node() : data(nullptr), next(nullptr) {};

			node(const T &d, node *n) : data(d), next(n) {};
		};

	private:
		ext::vector<node> buffer;
		size_t first = 0;
		size_t last = 0;

	private:
		void internal_push_back(const T &item) {
			buffer.push_back(node(item, nullptr));

			if (!last) {
				buffer.at(last).next = &(buffer.at(buffer.size() - 1));
			}

			last = buffer.size() - 1;
		};

	public:
		linked_list() = default;

		explicit linked_list(size_t size) : buffer(size) {};

		// ************
		// * Capacity *
		// ************
		bool empty() noexcept { return buffer.empty(); };

		size_t size() noexcept { return buffer.size(); };

		size_t capacity() const noexcept { return buffer.capacity(); };

		size_t max_capacity() const noexcept { return buffer.max_capacity(); };

		// *************
		// * Modifiers *
		// *************
		void clear() {
			buffer.clear();
		};

		void insert(size_t i, const T &item) {
			// TODO
		};

		template<class... Args>
		void emplace(Args &&... args) {
			// TODO
		};

		void remove(size_t i) {
			// TODO
		};

		void push_back(const T &item) {
			// TODO
		};

		void push_back(const T &&item) {
			// TODO
		};

		template<class... Args>
		void emplace_front(Args &&... args) {
			// TODO
		};

		void push_front(const T &item) {
			// TODO
		};

		void push_front(const T &&item) {
			// TODO
		};

		void pop_back() {
			// TODO
		};

		void pop_front() {
			// TODO
		};
	};
}

#endif //EXT_LINKED_LIST_HPP

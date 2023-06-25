#ifndef EXT_VECTOR_HPP
#define EXT_VECTOR_HPP

#include <iterator>
#include <utility>
#include <stdexcept>

#define EXT_VECTOR_ASSERT_INDEX(i)  if (i >= this->item_counter) { throw std::runtime_error("val out of range"); }

#define EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T) std::is_trivially_destructible<T>::value
#define EXT_VECTOR_NOTHROW_MOVE_DESTRUCTIBLE(T) std::is_nothrow_move_constructible<T>::value

#define EXT_VECTOR_BUFFER_INIT(size) (T *) ::operator new(size * sizeof(T))
#define EXT_VECTOR_BUFFER_DELETE ::operator delete(this->buffer)
#define EXT_VECTOR_SIZE(x) (2>x?2:x)

#define EXT_VECTOR_SCALE_FACTOR 1.6

namespace ext {
    /**
     * A dynamic sized container type storing T's
     * @tparam T - Type to store
     */
    template<class T>
    class vector {
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
        size_t buffer_size;
        size_t item_counter;
        T *buffer = nullptr;

        /**
         * Changes the size of the buffer
         * @warning Size is not validated, this can lead to memory leaks if Vector&lt;T&gt;::_internal_resize(size_t)
         *          is called with newSize &lt; buffer_size
         * @param newSize
         */
        void _internal_resize(size_t newSize) {
            vector<T> temp(newSize);
            this->_internal_simple_copy_vector_to(temp);
            temp.swap(*this);
        };

        /**
         * Changes the size of the buffer on demand to make room for more items
         */
        void _internal_resize_on_demand(size_t newItems) {
            if (item_counter + newItems > buffer_size) {
                double newSize = buffer_size;
                while ((size_t) newSize < item_counter + newItems) {
                    newSize *= EXT_VECTOR_SCALE_FACTOR;
                }

                this->_internal_resize((size_t)
                                               newSize);
            }
        };

        /**
         * Copies an item at index, not destructing any existing element or shifting existing elements to the side
         * @warning no bounds checking
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        void _internal_copy_put(size_t index, const T &item) {
            new(this->buffer + index) T(item);
            this->item_counter += 1;
        };

        /**
         * Moves an item at index, not destructing any existing element or shifting existing elements to the side
         * @warning no bounds checking
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        void _internal_move_put(size_t index, T &&item) {
            new(this->buffer + index) T(std::move(item));
            this->item_counter += 1;
        };

        /**
         * Moves an item at index, not destructing any existing element or shifting existing elements to the side
         * @warning no bounds checking
         * @tparam Args - Unknown amount of arguments and their types
         * @param index - Index of new item
         * @param args - Argument(s) for the constructor
         * @return Decides which methods get generated
         */
        template<typename... Args>
        void _internal_emplace_put(size_t index, Args &&... args) {
            new(this->buffer + index) T(std::move(args)...);
            this->item_counter += 1;
        }

        /**
         * Moves or copies an item from ori to dst <br>
         * Move or copy are dependent on if the datatype stored in the Vector is nothrow move constructable or not <br>
         * Note that for a copy the item is now duplicate at ori and dst, for move ori is now an "empty" value
         * @warning bounds are not checked
         * @param ori - Origin of item to be copied or moved
         * @param dst - Destination of move or copy
         * @return Decides which method gets generated
         */
        void
        _internal_move_or_copy_from_to(size_t ori, size_t dst) {
            if (EXT_VECTOR_NOTHROW_MOVE_DESTRUCTIBLE(T)) {
                new(this->buffer + dst) T(std::move(this->buffer[ori]));
            } else {
                new(this->buffer + dst) T(this->buffer[ori]);
            }
        };

        /**
         * Copy or move copy the current Vector into the destination Vector <br>
         * Move or copy are dependent on if the datatype stored in the Vector is nothrow move constructable or not
         * @param dst - Destination Vector
         * @return Decides which methods gets generated
         */
        void _internal_simple_copy_vector_to(vector<T> &dst) {
            for (size_t i = 0; i < this->item_counter; i += 1) {
                if (EXT_VECTOR_NOTHROW_MOVE_DESTRUCTIBLE(T)) {
                    dst._internal_move_put(dst.item_counter, std::move(this->buffer[i]));
                } else {
                    dst._internal_copy_put(this->item_counter, this->buffer[i]);
                }
            }
        };

        /**
         * Destructs the items in the Vector if they are not trivial destructible, else just sets the item counter to 0
         * @return Decides which methods get generated
         */
        void _internal_clear_items() {
            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                for (size_t i = 0; i < this->item_counter; i += 1) {
                    this->buffer[i].~T();
                }
            }

            this->item_counter = 0;
        };

        /**
         * Copies a Vector into the current Vector; tries to avoid new buffer allocation if enough space has already
         * been allocated to copy the Vector, else it allocates a new buffer <br>
         * Destructs items of current Vector if not trivial destructible
         * @param vec - Vector to be copied
         * @return Decides which methods get generated
         */
        void _internal_copy_assign(const vector<T> &vec) {
            this->_internal_clear_items();

            // if the buffer has enough memory allocated simply copy the elements
            // no new memory allocation
            if (buffer_size >= vec.item_counter) {
                for (size_t i = 0; i < vec.item_counter; i += 1) {
                    this->_internal_copy_put(this->item_counter, vec.buffer[i]);
                }
            } else {
                // if not enough memory has been allocated we need to allocate more
                // aka. create new Vector of the Vector we want to copy and swap the current one with it
                // TODO is this falsy? move semantics breached?
                vector<T> temp(vec);
                temp.swap(*this);
            }
        };

        /**
         * Erases the item at index and shifts all trailing items one to the left <br>
         * Destructs item if not trivial destructible
         * @warning no bounds checking
         * @param index - Item to be erased
         * @return Decides which methods get generated
         */
        void _internal_shift_left(size_t index) {
            this->item_counter -= 1;
            for (size_t i = index; i < this->item_counter; i += 1) {
                if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                    this->buffer[i].~T();
                }
                this->_internal_move_or_copy_from_to(i + 1, i);
            }
            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[this->item_counter].~T();
            }
        };

        /**
         * Shifts items right of index in Vector offset to the right
         * Destructs items if not trivial destructible
         * @warning no bounds checking
         * @param index - Starting point for shift
         * @param offset - Amount to shift
         * @return Decides which methods get generated
         */
        void _internal_shift_right(size_t index, size_t offset) {
            for (size_t i = this->item_counter - 1; i >= index; i -= 1) {
                this->_internal_move_or_copy_from_to(i, i + offset);
                if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                    this->buffer[i].~T();
                }
            }
        };

        /**
         * Inserts an item at index, copy shifting already existing items to the right
         * using Vector&lt;T&gt;::_internal_shift_right&lt;T&gt;(size_t) <br>
         * Destructs items if not trivial destructible
         * @warning no bounds checking
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        void _internal_copy_insert(size_t index, const T &item) {
            this->_internal_shift_right(index, 1);

            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[index].~T();
            }
            new(this->buffer + index) T(item);
            this->item_counter += 1;
        };

        /**
         * Same as Vector&lt;T&gt;::_internal_copy_insert(const T &, size_t) but moving instead of copying
         * @warning no bounds checking
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        void _internal_move_insert(size_t index, T &&item) {
            this->_internal_shift_right(index, 1);

            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[index].~T();
            }
            new(this->buffer + index) T(std::move(item));
            this->item_counter += 1;
        };

        /**
         * Constructs item at index in buffer, shifting already existing items one to the right
         * using Vector&lt;T&gt;::_internal_shift_right&lt;T&gt;(size_t) <br>
         * Destructs items if not trivial destructible
         * @warning no bounds checking
         * @tparam Args - Unknown amount of arguments and their type
         * @param index - Index of new item
         * @param args - Arguments for the constructor
         * @return Decides which methods get generated
         */
        template<class... Args>
        void _internal_emplace(size_t index, Args &&... args) {
            this->_internal_shift_right(index, 1);

            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[index].~T();
            }
            new(this->buffer + index) T(std::move(args)...);
            this->item_counter += 1;
        };

        /**
         * Removes the last item from the Vector <br>
         * Destructs the item if not trivial destructible
         * @return Decides which methods get generated
         */
        void _internal_pop_back() {
            this->item_counter -= 1;
            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[this->item_counter].~T();
            }
        };

        /**
         * Copies item and replaces index with it <br>
         * Destructs the item if not trivial destructible
         * @param index - Index to replace
         * @param item - Item to replace with
         * @return Decides which methods get generated
         */
        void _internal_copy_replace(size_t index, const T &item) {
            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[index].~T();
            }
            new(this->buffer + index) T(item);
        };

        /**
         * Same as Vector&lt;T&gt;::_internal_copy_replace&lt;T&gt;(size_t, const T&) but with moving
         */
        void _internal_move_replace(size_t index, T &&item) {
            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[index].~T();
            }
            new(this->buffer + index) T(std::move(item));
        };

        /**
         * Same as Vector&lt;T&gt;::_internal_copy_replace&lt;T&gt;(size_t, const T&) but with emplacing
         */
        template<class... Args>
        void
        _internal_emplace_replace(size_t index, Args &&... args) {
            if (!EXT_VECTOR_TRIVIAL_DESTRUCTIBLE(T)) {
                this->buffer[index].~T();
            }
            new(this->buffer + index) T(std::move(args)...);
        };

        /**
         * Swaps two items positions in the Vector <br>
         * Destructs the items if not trivial destructible
         * @warning destruction can throw and no bounds checking
         * @param a - Index of first item
         * @param b - Index of second item
         * @return Decides which methods get generated
         */
        void _internal_swap_items_init(size_t a, size_t b) {
            if (EXT_VECTOR_NOTHROW_MOVE_DESTRUCTIBLE(T)) {
                T temp = T(std::move(this->buffer[a]));

                this->_internal_move_replace(a, std::move(this->buffer[b]));
                this->_internal_move_replace(b, std::move(temp));
            } else {
                T temp = T(this->buffer[a]);

                this->_internal_copy_replace(a, this->buffer[b]);
                this->_internal_copy_replace(b, temp);
            }
        };

    public:
        /**
         * Default constructor <br>
         * The Vector \b always allocates and has space for \b at \b least two items out of ease of implementing
         * and logical use for a list type
         */
        vector() : buffer_size(2), item_counter(0), buffer(EXT_VECTOR_BUFFER_INIT(2)) {};

        /**
         * Constructs Vector of size MAX(2, size)
         * @param size - Size of Vector
         */
        vector(size_t size) : buffer_size(EXT_VECTOR_SIZE(size)), item_counter(0),
                              buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {};

        /**
         * Constructs Vector of size MAX(2, size) with each element being initialized with default_v
         * @param size - Size of Vector
         * @param default_v - Default value for each element
         */
        vector(size_t size, const T &default_v) : buffer_size(EXT_VECTOR_SIZE(size)), item_counter(0),
                                                  buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {
            for (size_t i = 0; i < this->buffer_size; i += 1) {
                this->_internal_copy_put(i, default_v);
            }
        };


        /**
         * Copy constructs a Vector from a given Vector
         * @param vec - Vector to be copied
         */
        vector(const vector<T> &vec) : buffer_size(vec.buffer_size), item_counter(0),
                                       buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {
            try {
                this->_internal_copy_assign(vec);
            } catch (...) {
                this->~vector();

                throw;
            }
        };

        /**
         * Move constructs Vector from given Vector
         * @param vec - Vector to be moved
         */
        vector(vector<T> &&vec) noexcept: buffer_size(0), item_counter(0), buffer(nullptr) {
            vec.swap(*this);
        };

        /**
         * Constructs Vector from initializer list <br>
         * Allocates space for at least two items in all cases
         * @param list - List of items
         */
        vector(std::initializer_list<T> list) : buffer_size(EXT_VECTOR_SIZE(list.size())), item_counter(0),
                                                buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {
            for (auto item: list) {
                this->_internal_copy_put(this->item_counter, item);
            }
        };

        /**
         * Constructs Vector from an iterator range <br>
         * Allocates space for at least two items in all cases
         * @param start - Start of range
         * @param stop - End of range
         */
        vector(iterator start, iterator stop) : buffer_size(EXT_VECTOR_SIZE(std::distance(start, stop))),
                                                item_counter(0), buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {
            while (start != stop) {
                this->_internal_copy_put(this->item_counter, *start);
                start += 1;
            }
        };

        /**
         * Destructor calls, if necessary, the destructor on all items of the Vector and then deletes the buffer
         */
        ~vector() {
            if (this->buffer != nullptr) {
                this->_internal_clear_items();
                EXT_VECTOR_BUFFER_DELETE;
                this->buffer = nullptr;
            }
        };

        /**
         * Copy assignment operator
         * @param vec - Vector to be copied
         * @return Self
         */
        vector<T> &operator=(const vector<T> &vec) {
            if (this != &vec) {
                this->_internal_copy_assign(vec); // TODO no catch?
            }

            return *this;
        };

        /**
         * Move assignment operator
         * @param vec - Vector to be moved
         * @return Self
         */
        vector<T> &operator=(vector<T> &&vec) noexcept {
            if (this != &vec) {
                vec.swap(*this);
            }

            return *this;
        };

        /**
         * Initializer list assignment operator
         * @param list - Initializer list
         * @return Self
         */
        vector<T> &operator=(std::initializer_list<T> list) {
            this->_internal_clear_items();
            this->_internal_resize_on_demand(list.size());

            for (auto item: list) {
                this->_internal_copy_put(this->item_counter, item);
            }

            return *this;
        };

        // ***************
        // * Item Access *
        // ***************
        /**
         * Returns item reference at index
         * @warning Doesn't check bounds
         * @param index - Index
         * @return Item reference
         */
        T &operator[](size_t index) {
            return this->buffer[index];
        };

        /**
         * Returns const item reference at index
         * @warning Doesn't check bounds
         * @param index - Index
         * @return Item reference
         */
        const T &operator[](size_t index) const {
            return this->buffer[index];
        };

        /**
         * Returns item reference at index
         * @param index - Index
         * @return Item reference
         */
        T &at(size_t index) {
            EXT_VECTOR_ASSERT_INDEX(index)

            return this->buffer[index];
        };

        /**
         * Returns const item reference at index
         * @param index - Index
         * @return Item reference
         */
        T &at(size_t index) const {
            EXT_VECTOR_ASSERT_INDEX(index)

            return this->buffer[index];
        };

        T *data() noexcept { return this->buffer; };

        T &front() { return this->buffer[0]; };

        T &front() const { return this->buffer[0]; };

        T &back() { return this->buffer[this->item_counter - 1]; };

        T &back() const { return this->buffer[this->item_counter - 1]; };

        // *************
        // * Iterators *
        // *************
        iterator begin() { return this->buffer; };

        riterator rbegin() { return riterator(end()); };

        const_iterator begin() const { return this->buffer; };

        const_riterator rbegin() const { return const_riterator(end()); };

        iterator end() { return this->buffer + this->item_counter; };

        riterator rend() { return riterator(begin()); };

        const_iterator end() const { return this->buffer + this->item_counter; };

        const_riterator rend() const { return const_riterator(begin()); };

        const_iterator cbegin() const { return begin(); };

        const_riterator crbegin() const { return rbegin(); };

        const_iterator cend() const { return end(); };

        const_riterator crend() const { return rend(); };

        // ************
        // * Capacity *
        // ************
        bool empty() noexcept { return this->item_counter == 0; };

        size_t size() const noexcept { return this->item_counter; };

        size_t capacity() const noexcept { return this->buffer_size; };

        inline size_t max_capacity() const noexcept { return SIZE_MAX; };

        /**
         * Allocates more memory for the buffer <br>
         * If newSize is smaller than the current size nothing happens
         * @param newSize - Size of buffer
         */
        void reserve(size_t newSize) {
            if (newSize > this->buffer_size) {
                this->_internal_resize(newSize);
            }
        };

        /**
         * Frees all memory the buffer allocated which is not currently occupied by items
         */
        void shrink_to_fit() {
            vector<T> temp(EXT_VECTOR_SIZE(this->item_counter));
            this->_internal_simple_copy_vector_to(temp);
            temp.swap(*this);
        };

        // *************
        // * Modifiers *
        // *************
        /**
         * Clears the Vector <br>
         * Destructs all items if not trivial destructible
         */
        void clear() {
            this->_internal_clear_items();
        };

        /**
         * Erases item at index <br>
         * Destructs item if not trivial destructible
         * @param index - Index
         */
        void erase(size_t index) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_shift_left(index);
        };

        /**
         * Copy inserts item at index
         * @param index - Index
         * @param item - Item to be inserted
         */
        void insert(size_t index, const T &item) {
            EXT_VECTOR_ASSERT_INDEX(index)
            this->_internal_resize_on_demand(1);

            this->_internal_copy_insert(index, item);
        };

        /**
         * Move inserts item at index
         * @param index - Index
         * @param item - Item to be inserted
         */
        void insert(size_t index, T &&item) {
            EXT_VECTOR_ASSERT_INDEX(index)
            this->_internal_resize_on_demand(1);

            this->_internal_move_insert(index, std::move(item));
        };

        /**
         * Inserts an initializer list at index
         * @param index - Index
         * @param list - List to be pushed
         */
        void insert(size_t index, std::initializer_list<T> list) {
            this->_internal_resize_on_demand(list.size());
            this->_internal_shift_right(index, list.size());

            size_t i = index;
            for (auto item: list) {
                this->_internal_copy_put(i, item);
                i += 1;
            }
        };

        /**
         * Copy inserts a vector at index
         * @param index - Index
         * @param vec - Vector to insert
         */
        void insert(size_t index, vector<T> &vec) {
            this->_internal_resize_on_demand(vec.size());
            this->_internal_shift_right(index, vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_copy_put(index + i, vec[i]);
            }
        };

        /**
         * Move inserts a vector at index
         * @param index - Index
         * @param vec - Vector to insert
         */
        void insert(size_t index, vector<T> &&vec) {
            this->_internal_resize_on_demand(vec.size());
            this->_internal_shift_right(index, vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_move_put(index + i, std::move(vec[i]));
            }
        };

        /**
         * Inserts an iterator range at index
         * @param index - Index
         * @param start - Start of range
         * @param stop - End of range
         */
        void insert(size_t index, iterator start, iterator stop) {
            size_t size = std::distance(start, stop);
            this->_internal_resize_on_demand(size);
            this->_internal_shift_right(index, size);

            size_t i = index;
            while (start != stop) {
                this->_internal_copy_put(i, *start);
                start += 1;
                i += 1;
            }
        }

        /**
         * Copy pushes item at the end of the Vector
         * @param item - Item to be pushed
         */
        void push_back(const T &item) {
            this->_internal_resize_on_demand(1);

            this->_internal_copy_put(this->item_counter, item);
        };

        /**
         * Move pushes item at the end of the Vector
         * @param item - Item to be pushed
         */
        void push_back(T &&item) {
            this->_internal_resize_on_demand(1);

            this->_internal_move_put(this->item_counter, std::move(item));
        };

        /**
         * Pushes an initializer list at the end of the vector
         * @param list - List to be pushed
         */
        void push_back(std::initializer_list<T> list) {
            this->_internal_resize_on_demand(list.size());

            for (auto item: list) {
                this->_internal_copy_put(this->item_counter, item);
            }
        };

        /**
         * Copy pushes a vector at the end of the vector
         * @param vec - Vector to push
         */
        void push_back(vector<T> &vec) {
            this->_internal_resize_on_demand(vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_copy_put(this->item_counter, vec[i]);
            }
        };

        /**
         * Move pushes a vector at the end of the vector
         * @param vec - Vector to push
         */
        void push_back(vector<T> &&vec) {
            this->_internal_resize_on_demand(vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_move_put(this->item_counter, std::move(vec[i]));
            }
        };

        /**
         * Pushes an iterator range at the stop of the vector
         * @param start - Start of range
         * @param stop - End of range
         */
        void push_back(iterator start, iterator stop) {
            size_t size = std::distance(start, stop);
            this->_internal_resize_on_demand(size);

            while (start != stop) {
                this->_internal_copy_put(this->item_counter, *start);
                start += 1;
            }
        }

        /**
         * Removes the last item of the Vector
         */
        void pop_back() {
            this->_internal_pop_back();
        };

        /**
         * Swaps the content of two vectors
         * @param vec - Vector to swap with
         */
        void swap(vector<T> &vec) {
            std::swap(this->buffer_size, vec.buffer_size);
            std::swap(this->item_counter, vec.item_counter);
            std::swap(this->buffer, vec.buffer);
        };

        /**
         * Swaps two items in the Vector
         * @param a - Index of item A
         * @param b - Index of item B
         */
        void swap_items(size_t a, size_t b) {
            EXT_VECTOR_ASSERT_INDEX(a)
            EXT_VECTOR_ASSERT_INDEX(b)

            this->_internal_swap_items_init(a, b);
        };

        /**
         * Resizes the Vector
         * @param size - New item count of Vector
         */
        void resize(size_t size) {
            for (size_t i = this->item_counter; i > size; i -= 1) {
                this->_internal_pop_back();
            }
        };

        /**
         * Constructs item in place at index, shifting already existing item and all trailing items to the right
         * @tparam Args - Unknown amount of arguments and their type
         * @param index - Index
         * @param args - Constructor arguments
         */
        template<class... Args>
        void emplace(size_t index, Args &&... args) {
            EXT_VECTOR_ASSERT_INDEX(index)
            this->_internal_resize_on_demand(1);

            this->_internal_emplace(index, std::move(args)...);
        };

        /**
         * Constructs item in place at end of Vector
         * @tparam Args - Unknown amount of arguments and their type
         * @param args - Constructor arguments
         */
        template<class... Args>
        void emplace_back(Args &&... args) {
            this->_internal_resize_on_demand(1);

            this->_internal_emplace_put(this->item_counter, std::move(args)...);
        };

        /**
         * Copies item and replaces index with it
         * @param index - Index to replace
         * @param item - Item to replace with
         */
        void replace(size_t index, const T &item) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_copy_replace(index, item);
        };

        /**
         * Moves item and replaces index with it
         * @param index - Index to replace
         * @param item - Item to replace with
         */
        void replace(size_t index, T &&item) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_move_replace(index, std::move(item));
        };

        /**
         * Constructs item and replaces index with it
         * @tparam Args - Unknown amount of arguments and their type
         * @param index - Index to replace
         * @param args - Constructor arguments
         */
        template<class... Args>
        void replace(size_t index, Args &&... args) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_emplace_replace(index, std::move(args)...);
        };

        // ************************
        // * Non-Member Functions *
        // ************************
        /**
         * Returns if two vectors items are the same
         * @param vec - Vector to compare to
         * @return
         */
        bool operator==(const vector<T> &vec) {
            if (this->item_counter != vec.item_counter) {
                return false;
            }

            for (size_t i = 0; i < this->item_counter; ++i) {
                if (this->buffer[i] != vec.buffer[i]) {
                    return false;
                }
            }
            return true;
        };

        /**
         * Returns if two vectors items are not the same
         * @param vec - Vector to compare to
         * @return
         */
        bool operator!=(const vector<T> &vec) {
            return !(*this == vec);
        };
    };
}

#endif
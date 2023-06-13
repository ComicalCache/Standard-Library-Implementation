#ifndef EXT_VECTOR_HPP
#define EXT_VECTOR_HPP

#include <iterator>
#include <utility>
#include <stdexcept>

#define EXT_VECTOR_ASSERT_INDEX(i)  if (i >= item_counter) { throw std::runtime_error("val out of range"); }

#define EXT_VECTOR_INTERNAL_SIMPLE_COPY_VECTOR_TO_RETURN(b) typename std::enable_if<std::is_nothrow_move_constructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_MOVE_COPY_RETURN(b) typename std::enable_if<std::is_nothrow_move_constructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_SWAP_ITEMS_RETURN(b) typename std::enable_if<std::is_nothrow_move_constructible<X>::value == b>::type

#define EXT_VECTOR_INTERNAL_CLEAR_ITEMS_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_SHIFT_LEFT_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_SHIFT_RIGHT_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_INSERT_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_POP_BACK_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_EMPLACE_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_COPY_REPLACE_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_MOVE_REPLACE_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type
#define EXT_VECTOR_INTERNAL_EMPLACE_REPLACE_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type

#define EXT_VECTOR_BUFFER_INIT(size) (T *) ::operator new(size * sizeof(T))
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
        T *buffer;

        /**
         * Changes the size of the buffer
         * @warning Size is not validated, this can lead to memory leaks if Vector&lt;T&gt;::_internal_resize(size_t)
         *          is called with newSize &lt; buffer_size
         * @param newSize
         */
        void _internal_resize(size_t newSize) {
            vector<T> temp(newSize);
            this->_internal_simple_copy_vector_to<T>(temp);
            temp.swap(*this);
            temp.~vector();
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

                this->_internal_resize((size_t) newSize);
            }
        };

        /**
         * Copies item to end of buffer and increments the item counter
         * @param item - Item to be pushed
         */
        void _internal_push_back(const T &item) {
            new(buffer + item_counter) T(item);
            item_counter += 1;
        };

        /**
         * Moves item to end of buffer and increments the item counter.
         * @param item - Item to be moved
         */
        void _internal_move_back(T &&item) {
            new(buffer + item_counter) T(std::move(item));
            item_counter += 1;
        };

        /**
         * Constructs item at the end of the buffer and increments the item counter
         * @tparam Args - Unknown amount of arguments and their types
         * @param args - Argument(s) for the constructor
         */
        template<class... Args>
        void _internal_emplace_back(Args &&... args) {
            new(buffer + item_counter) T(std::move(args)...);
            item_counter += 1;
        };

        /**
         * Moves or copies an item from ori to dst <br>
         * Move or copy are dependent on if the datatype stored in the Vector is nothrow move constructable or not <br>
         * Note that for a copy the item is now duplicate at ori and dst, for move ori is now an "empty" value
         * @warning bounds are not checked
         * @tparam X  - Datatype of buffer stored in Vector
         * @param ori - Origin of item to be copied or moved
         * @param dst - Destination of move or copy
         * @return Decides which method gets generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_MOVE_COPY_RETURN(false) _internal_move_or_copy(size_t ori, size_t dst) {
            new(buffer + dst) T(buffer[ori]);
        };

        template<class X>
        EXT_VECTOR_INTERNAL_MOVE_COPY_RETURN(true) _internal_move_or_copy(size_t ori, size_t dst) {
            new(buffer + dst) T(std::move(buffer[ori]));
        };

        /**
         * Copy or move copy the current Vector into the destination Vector <br>
         * Move or copy are dependent on if the datatype stored in the Vector is nothrow move constructable or not
         * @tparam X - Datatype of buffer stored in Vector
         * @param dst - Destination Vector
         * @return Decides which methods gets generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_SIMPLE_COPY_VECTOR_TO_RETURN(false) _internal_simple_copy_vector_to(vector<T> &dst) {
            for (size_t i = 0; i < item_counter; i += 1) {
                dst._internal_push_back(buffer[i]);
            }
        };

        template<class X>
        EXT_VECTOR_INTERNAL_SIMPLE_COPY_VECTOR_TO_RETURN(true) _internal_simple_copy_vector_to(vector<T> &dst) {
            for (size_t i = 0; i < item_counter; i += 1) {
                dst._internal_move_back(std::move(buffer[i]));
            }
        };

        /**
         * Destructs the items in the Vector if they are not trivial destructible, else just sets the item counter to 0
         * @tparam X - Datatype of buffer stored in Vector
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_CLEAR_ITEMS_RETURN(false) _internal_clear_items() {
            for (size_t i = 0; i < item_counter; i += 1) {
                buffer[i].~T();
            }
            item_counter = 0;
        };

        template<class X>
        EXT_VECTOR_INTERNAL_CLEAR_ITEMS_RETURN(true) _internal_clear_items() {
            item_counter = 0;
        };

        /**
         * Copies a Vector into the current Vector; tries to avoid new buffer allocation if enough space has already
         * been allocated to copy the Vector, else it allocates a new buffer <br>
         * Destructs items of current Vector if not trivial destructible
         * @param vec - Vector to be copied
         * @return Decides which methods get generated
         */
        void _internal_copy_assign(const vector<T> &vec) {
            this->_internal_clear_items<T>();

            // if the buffer has enough memory allocated simply copy the elements
            // no new memory allocation
            if (buffer_size >= vec.item_counter) {
                for (size_t i = 0; i < vec.item_counter; i += 1) {
                    this->_internal_push_back(vec.buffer[i]);
                }
            } else {
                // if not enough memory has been allocated we need to allocate more
                // aka. create new Vector of the Vector we want to copy and swap the current one with it
                vector<T> temp(vec);
                temp.swap(*this);
                temp.~vector();
            }
        };

        /**
         * Erases the item at index and shifts all trailing items one to the left <br>
         * Destructs item if not trivial destructible
         * @warning no bounds checking
         * @tparam X - Datatype of buffer stored in Vector
         * @param index - Item to be erased
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_SHIFT_LEFT_RETURN(false) _internal_shift_left(size_t index) {
            item_counter -= 1;
            for (size_t i = index; i < item_counter; i += 1) {
                buffer[i].~T();
                this->_internal_move_or_copy<T>(i + 1, i);
            }
            buffer[item_counter].~T();
        };

        template<class X>
        EXT_VECTOR_INTERNAL_SHIFT_LEFT_RETURN(true) _internal_shift_left(size_t index) {
            item_counter -= 1;
            for (size_t i = index; i < item_counter; i += 1) {
                this->_internal_move_or_copy<T>(i + 1, i);
            }
        };

        /**
         * Shifts items right of index in Vector offset to the right
         * Destructs items if not trivial destructible
         * @warning no bounds checking
         * @tparam X - Datatype of buffer stored in Vector
         * @param index - Starting point for shift
         * @param offset - Amount to shift
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_SHIFT_RIGHT_RETURN(false) _internal_shift_right(size_t index, size_t offset) {
            for (size_t i = item_counter - 1; i >= index; i -= 1) {
                this->_internal_move_or_copy<T>(i, i + offset);
                buffer[i].~T();
            }
        };

        template<class X>
        EXT_VECTOR_INTERNAL_SHIFT_RIGHT_RETURN(true) _internal_shift_right(size_t index, size_t offset) {
            for (size_t i = item_counter - 1; i >= index; i -= 1) {
                this->_internal_move_or_copy<T>(i, i + offset);
            }
        };

        /**
         * Inserts an item at index, copy shifting already existing items to the right
         * using Vector&lt;T&gt;::_internal_shift_right&lt;T&gt;(size_t) <br>
         * Destructs items if not trivial destructible
         * @warning no bounds checking
         * @tparam X - Datatype of buffer stored in Vector
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_INSERT_RETURN(false) _internal_copy_insert(size_t index, const T &item) {
            this->_internal_shift_right<T>(index, 1);

            buffer[index].~T();
            new(buffer + index) T(item);
            item_counter += 1;
        };

        template<class X>
        EXT_VECTOR_INTERNAL_INSERT_RETURN(true) _internal_copy_insert(size_t index, const T &item) {
            this->_internal_shift_right<T>(index, 1);

            new(buffer + index) T(item);
            item_counter += 1;
        };

        /**
         * Same as Vector&lt;T&gt;::_internal_copy_insert(const T &, size_t) but moving instead of copying
         * @warning no bounds checking
         * @tparam X - Datatype of buffer stored in Vector
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_INSERT_RETURN(false) _internal_move_insert(size_t index, T &&item) {
            this->_internal_shift_right<T>(index, 1);

            buffer[index].~T();
            new(buffer + index) T(std::move(item));
            item_counter += 1;
        };

        template<class X>
        EXT_VECTOR_INTERNAL_INSERT_RETURN(true) _internal_move_insert(size_t index, T &&item) {
            this->_internal_shift_right<T>(index, 1);

            new(buffer + index) T(std::move(item));
            item_counter += 1;
        };

        /**
         * Constructs item at index in buffer, shifting already existing items one to the right
         * using Vector&lt;T&gt;::_internal_shift_right&lt;T&gt;(size_t) <br>
         * Destructs items if not trivial destructible
         * @warning no bounds checking
         * @tparam X - Datatype of buffer stored in Vector
         * @tparam Args - Unknown amount of arguments and their type
         * @param index - Index of new item
         * @param args - Arguments for the constructor
         * @return Decides which methods get generated
         */
        template<class X, class... Args>
        EXT_VECTOR_INTERNAL_EMPLACE_RETURN(false) _internal_emplace(size_t index, Args &&... args) {
            this->_internal_shift_right<T>(index, 1);

            buffer[index].~T();
            new(buffer + index) T(std::move(args)...);
            item_counter += 1;
        };

        template<class X, class... Args>
        EXT_VECTOR_INTERNAL_EMPLACE_RETURN(true) _internal_emplace(size_t index, Args &&... args) {
            this->_internal_shift_right<T>(index, 1);

            new(buffer + index) T(std::move(args)...);
            item_counter += 1;
        };

        /**
         * Copies an item at index, not destructing any existing element or shifting existing elements to the side
         * @warning no bounds checking
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        void _internal_copy_put(size_t index, const T &item) {
            new(buffer + index) T(item);
            item_counter += 1;
        };

        /**
         * Moves an item at index, not destructing any existing element or shifting existing elements to the side
         * @warning no bounds checking
         * @param index - Index of new item
         * @param item - Item to be inserted
         * @return Decides which methods get generated
         */
        void _internal_move_put(size_t index, T &&item) {
            new(buffer + index) T(std::move(item));
            item_counter += 1;
        };

        /**
         * Removes the last item from the Vector <br>
         * Destructs the item if not trivial destructible
         * @tparam X - Datatype of buffer stored in Vector
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_POP_BACK_RETURN(false) _internal_pop_back() {
            item_counter -= 1;
            buffer[item_counter].~T();
        };

        template<class X>
        EXT_VECTOR_INTERNAL_POP_BACK_RETURN(true) _internal_pop_back() {
            item_counter -= 1;
        };

        /**
         * Copies item and replaces index with it <br>
         * Destructs the item if not trivial destructible
         * @tparam X - Datatype of buffer stored in Vector
         * @param index - Index to replace
         * @param item - Item to replace with
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_COPY_REPLACE_RETURN(false) _internal_copy_replace(size_t index, const T &item) {
            buffer[index].~T();
            new(buffer + index) T(item);
        };

        template<class X>
        EXT_VECTOR_INTERNAL_COPY_REPLACE_RETURN(true) _internal_copy_replace(size_t index, const T &item) {
            new(buffer + index) T(item);
        };

        /**
         * Same as Vector&lt;T&gt;::_internal_copy_replace&lt;T&gt;(size_t, const T&) but with moving
         */
        template<class X>
        EXT_VECTOR_INTERNAL_MOVE_REPLACE_RETURN(false) _internal_move_replace(size_t index, T &&item) {
            buffer[index].~T();
            new(buffer + index) T(std::move(item));
        };

        template<class X>
        EXT_VECTOR_INTERNAL_MOVE_REPLACE_RETURN(true) _internal_move_replace(size_t index, T &&item) {
            new(buffer + index) T(std::move(item));
        };

        /**
         * Same as Vector&lt;T&gt;::_internal_copy_replace&lt;T&gt;(size_t, const T&) but with emplacing
         */
        template<class X, class... Args>
        EXT_VECTOR_INTERNAL_EMPLACE_REPLACE_RETURN(false) _internal_emplace_replace(size_t index, Args &&... args) {
            buffer[index].~T();
            new(buffer + index) T(std::move(args)...);
        };

        template<class X, class... Args>
        EXT_VECTOR_INTERNAL_EMPLACE_REPLACE_RETURN(true) _internal_emplace_replace(size_t index, Args &&... args) {
            new(buffer + index) T(std::move(args)...);
        };

        /**
         * Swaps two items positions in the Vector <br>
         * Destructs the items if not trivial destructible
         * @warning destruction can throw and no bounds checking
         * @tparam X - Datatype of buffer stored in Vector
         * @param a - Index of first item
         * @param b - Index of second item
         * @return Decides which methods get generated
         */
        template<class X>
        EXT_VECTOR_INTERNAL_SWAP_ITEMS_RETURN(false) _internal_swap_items_init(size_t a, size_t b) {
            T temp = T(buffer[a]);

            this->_internal_copy_replace<T>(a, buffer[b]);
            this->_internal_copy_replace<T>(b, temp);
        };

        template<class X>
        EXT_VECTOR_INTERNAL_SWAP_ITEMS_RETURN(true) _internal_swap_items_init(size_t a, size_t b) {
            T temp = T(std::move(buffer[a]));

            this->_internal_move_replace<T>(a, std::move(buffer[b]));
            this->_internal_move_replace<T>(b, std::move(temp));
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
        explicit vector(size_t size) : buffer_size(EXT_VECTOR_SIZE(size)), item_counter(0),
                                       buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {};

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
                this->_internal_push_back(item);
            }
        };

        /**
         * Constructs Vector from an iterator range <br>
         * Allocates space for at least two items in all cases
         * @tparam Iterator - The iterator type
         * @param start - Start of range
         * @param stop - End of range
         */
        template<typename Iterator>
        vector(Iterator start, Iterator stop) : buffer_size(EXT_VECTOR_SIZE(std::distance(start, stop))),
                                                item_counter(0), buffer(EXT_VECTOR_BUFFER_INIT(buffer_size)) {
            while (start != stop) {
                this->_internal_push_back(*start);
                start += 1;
            }
        };

        /**
         * Destructor calls, if necessary, the destructor on all items of the Vector and then deletes the buffer
         */
        ~vector() {
            if (buffer != nullptr) {
                this->_internal_clear_items<T>();
                ::operator delete(buffer);
                buffer = nullptr;
            }
        };

        /**
         * Copy assignment operator
         * @param vec - Vector to be copied
         * @return Self
         */
        vector<T> &operator=(const vector<T> &vec) {
            if (this != &vec) {
                this->_internal_copy_assign(vec); // ? no catch
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
            this->_internal_clear_items<T>();
            this->_internal_resize_on_demand(list.size());

            for (auto item: list) {
                this->_internal_push_back(item);
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
            return buffer[index];
        };

        /**
         * Returns const item reference at index
         * @warning Doesn't check bounds
         * @param index - Index
         * @return Item reference
         */
        const T &operator[](size_t index) const {
            return buffer[index];
        };

        /**
         * Returns item reference at index
         * @param index - Index
         * @return Item reference
         */
        T &at(size_t index) {
            EXT_VECTOR_ASSERT_INDEX(index)

            return buffer[index];
        };

        /**
         * Returns const item reference at index
         * @param index - Index
         * @return Item reference
         */
        T &at(size_t index) const {
            EXT_VECTOR_ASSERT_INDEX(index)

            return buffer[index];
        };

        T *data() noexcept { return buffer; };

        T &front() { return buffer[0]; };

        T &front() const { return buffer[0]; };

        T &back() { return buffer[item_counter - 1]; };

        T &back() const { return buffer[item_counter - 1]; };

        // *************
        // * Iterators *
        // *************
        iterator begin() { return buffer; };

        riterator rbegin() { return riterator(end()); };

        const_iterator begin() const { return buffer; };

        const_riterator rbegin() const { return const_riterator(end()); };

        iterator end() { return buffer + item_counter; };

        riterator rend() { return riterator(begin()); };

        const_iterator end() const { return buffer + item_counter; };

        const_riterator rend() const { return const_riterator(begin()); };

        const_iterator cbegin() const { return begin(); };

        const_riterator crbegin() const { return rbegin(); };

        const_iterator cend() const { return end(); };

        const_riterator crend() const { return rend(); };

        // ************
        // * Capacity *
        // ************
        bool empty() noexcept { return item_counter == 0; };

        size_t size() const noexcept { return item_counter; };

        size_t capacity() const noexcept { return buffer_size; };

        inline size_t max_capacity() const noexcept { return SIZE_MAX; };

        /**
         * Allocates more memory for the buffer <br>
         * If newSize is smaller than the current size nothing happens
         * @param newSize - Size of buffer
         */
        void reserve(size_t newSize) {
            if (newSize > buffer_size) {
                this->_internal_resize(newSize);
            }
        };

        /**
         * Frees all memory the buffer allocated which is not currently occupied by items
         */
        void shrink_to_fit() {
            vector<T> temp(EXT_VECTOR_SIZE(item_counter));
            this->_internal_simple_copy_vector_to<T>(temp);
            temp.swap(*this);

            if (std::is_nothrow_destructible<T>::value == true) {
                temp.~vector();
            }
        };

        // *************
        // * Modifiers *
        // *************
        /**
         * Clears the Vector <br>
         * Destructs all items if not trivial destructible
         */
        void clear() {
            this->_internal_clear_items<T>();
        };

        /**
         * Erases item at index <br>
         * Destructs item if not trivial destructible
         * @param index - Index
         */
        void erase(size_t index) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_shift_left<T>(index);
        };

        /**
         * Copy inserts item at index
         * @param index - Index
         * @param item - Item to be inserted
         */
        void insert(size_t index, const T &item) {
            EXT_VECTOR_ASSERT_INDEX(index)
            this->_internal_resize_on_demand(1);

            this->_internal_copy_insert<T>(index, item);
        };

        /**
         * Move inserts item at index
         * @param index - Index
         * @param item - Item to be inserted
         */
        void insert(size_t index, T &&item) {
            EXT_VECTOR_ASSERT_INDEX(index)
            this->_internal_resize_on_demand(1);

            this->_internal_move_insert<T>(index, std::move(item));
        };

        /**
         * Inserts an initializer list at index
         * @param index - Index
         * @param list - List to be pushed
         */
        void insert(size_t index, std::initializer_list<T> list) {
            this->_internal_resize_on_demand(list.size());
            this->_internal_shift_right<T>(index, list.size());

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
            this->_internal_shift_right<T>(index, vec.size());

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
            this->_internal_shift_right<T>(index, vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_move_put(index + i, std::move(vec[i]));
            }
        };

        /**
         * Inserts an iterator range at index
         * @tparam Iterator - The iterator type
         * @param index - Index
         * @param start - Start of range
         * @param stop - End of range
         */
        template<typename Iterator>
        void insert(size_t index, Iterator start, Iterator stop) {
            size_t size = std::distance(start, stop);
            this->_internal_resize_on_demand(size);
            this->_internal_shift_right<T>(index, size);

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

            this->_internal_push_back(item);
        };

        /**
         * Move pushes item at the end of the Vector
         * @param item - Item to be pushed
         */
        void push_back(T &&item) {
            this->_internal_resize_on_demand(1);

            this->_internal_move_back(std::move(item));
        };

        /**
         * Pushes an initializer list at the end of the vector
         * @param list - List to be pushed
         */
        void push_back(std::initializer_list<T> list) {
            this->_internal_resize_on_demand(list.size());

            for (auto item: list) {
                this->_internal_push_back(item);
            }
        };

        /**
         * Copy pushes a vector at the end of the vector
         * @param vec - Vector to push
         */
        void push_back(vector<T> &vec) {
            this->_internal_resize_on_demand(vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_push_back(vec[i]);
            }
        };

        /**
         * Move pushes a vector at the end of the vector
         * @param vec - Vector to push
         */
        void push_back(vector<T> &&vec) {
            this->_internal_resize_on_demand(vec.size());

            for (size_t i = 0; i < vec.size(); i += 1) {
                this->_internal_move_back(std::move(vec[i]));
            }
        };

        /**
         * Pushes an iterator range at the stop of the vector
         * @tparam Iterator - The iterator type
         * @param start - Start of range
         * @param stop - End of range
         */
        template<typename Iterator>
        void push_back(Iterator start, Iterator stop) {
            size_t size = std::distance(start, stop);
            this->_internal_resize_on_demand(size);

            while (start != stop) {
                this->_internal_push_back(*start);
                start += 1;
            }
        }

        /**
         * Removes the last item of the Vector
         */
        void pop_back() {
            this->_internal_pop_back<T>();
        };

        /**
         * Swaps the content of two vectors
         * @param vec - Vector to swap with
         */
        void swap(vector<T> &vec) {
            std::swap(buffer_size, vec.buffer_size);
            std::swap(item_counter, vec.item_counter);
            std::swap(buffer, vec.buffer);
        };

        /**
         * Swaps two items in the Vector
         * @param a - Index of item A
         * @param b - Index of item B
         */
        void swap_items(size_t a, size_t b) {
            EXT_VECTOR_ASSERT_INDEX(a)
            EXT_VECTOR_ASSERT_INDEX(b)

            this->_internal_swap_items_init<T>(a, b);
        };

        /**
         * Resizes the Vector
         * @param size - New item count of Vector
         */
        void resize(size_t size) {
            for (size_t i = item_counter; i > size; i -= 1) {
                this->_internal_pop_back<T>();
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

            this->_internal_emplace<T>(index, std::move(args)...);
        };

        /**
         * Constructs item in place at end of Vector
         * @tparam Args - Unknown amount of arguments and their type
         * @param args - Constructor arguments
         */
        template<class... Args>
        void emplace_back(Args &&... args) {
            this->_internal_resize_on_demand(1);

            this->_internal_emplace_back(std::move(args)...);
        };

        /**
         * Copies item and replaces index with it
         * @param index - Index to replace
         * @param item - Item to replace with
         */
        void replace(size_t index, const T &item) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_copy_replace<T>(index, item);
        };

        /**
         * Moves item and replaces index with it
         * @param index - Index to replace
         * @param item - Item to replace with
         */
        void replace(size_t index, T &&item) {
            EXT_VECTOR_ASSERT_INDEX(index)

            this->_internal_move_replace<T>(index, std::move(item));
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

            this->_internal_emplace_replace<T>(index, std::move(args)...);
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

            for (size_t i = 0; i < item_counter; ++i) {
                if (buffer[i] != vec.buffer[i]) {
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
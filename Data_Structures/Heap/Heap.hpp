#ifndef HEAP_LIBRARY_HPP
#define HEAP_LIBRARY_HPP

#include <Vector/Vector.hpp>

/**
 * Pre-made max heap function
 * @param T - Type of buffer stored in heap
 */
#define EXT_MAX_HEAP(T) [](const T &t1, const T &t2) { return t1 > t2; }
#define EXT_HEAP_SIZE(n) (size_t) n

namespace ext {
    template<class T>
    class heap {
    private:
        vector<T> buffer;

        /**
         * Sorting condition for heap
         */
        bool (*condition)(const T &, const T &) = [](const T &t1, const T &t2) {
            return t1 < t2;
        };

    private:
        /**
         * Swaps two items in the heap if the sorting condition applies
         * @warning no bounds checking
         * @param pos1 - Index of first item
         * @param pos2 - Index of second item
         * @return If items have been swapped
         */
        bool _internal_swap_on_demand(size_t pos1, size_t pos2) {
            if (condition(buffer[pos1], buffer[pos2])) {
                buffer.swap_items(pos1, pos2);
                return true;
            }
            return false;
        }

        /**
         * Returns the child that fits the condition
         * @warning no bounds checking
         * @param i1 - Index of child one
         * @param i2 - Index of child two
         * @return Child
         */
        inline size_t _internal_get_correct_child(size_t i1, size_t i2) {
            return condition(buffer[i1], buffer[i2]) ? i1 : i2;
        }

        /**
         * Sorts the latest added item in the heap
         */
        void _internal_push() {
            size_t pos = buffer.size() - 1;

            /*
             * While swaps happen and is not base item continue loop.
             * Shift to the right to divide by 2 to move up one layer, then subtract one
             * if even number to adjust for 0 based indexing to always get the left child
             * and compare them against its parent
             *
             * Example:
             *          0
             *      1       2
             *      ^
             *    3   4   5   6
             *    ^       ^
             *    always get these children
             */
            while (pos != 0 && this->_internal_swap_on_demand(pos, (pos = (pos >> 1) - (pos % 2 == 0)))) {
            }
        }

        /**
         * Rearranges the items after the base has been popped
         */
        void _internal_remove() {
            size_t pos = 1;
            size_t child;

            /*
             * pos: left child of parent
             * child: val of child that fits the condition (left or right)
             *
             * While swaps happen and there are two children left continue loop.
             * Get best suited children and swap with parent (pos >> 1) if the swap condition applies, then adjust
             * pos to the child of the child
             */
            while (buffer.size() > pos + 1 &&
                   this->_internal_swap_on_demand((child = this->_internal_get_correct_child(pos, pos + 1)),
                                                  pos >> 1)) {
                pos = (child << 1) + 1;
            }
            if (buffer.size() > pos) { this->_internal_swap_on_demand(pos, pos >> 1); }
        }

    public:
        heap() = default;

        /**
         * Constructor; pre-allocates memory for size items in the buffer
         * @param size - Amount of memory to allocate
         */
        explicit heap(size_t size) : buffer(size) {}

        /**
         * Constructor; pre-allocates memory for size items in buffer with custom sorting condition
         * @tparam FuncSig - Implicit return of function
         * @param size - Amount of memory to allocate
         * @param f - Custom sorting condition
         */
        template<class FuncSig>
        explicit heap(size_t size, FuncSig f) : buffer(size), condition(f) {}

        /**
         * Constructor; assigns custom sorting condition
         * @tparam FuncSig - Implicit return of function
         * @param f - Custom sorting condition
         */
        template<class FuncSig>
        explicit heap(FuncSig f) : buffer(), condition(f) {}

        /**
         * Constructor; initializes the heap with the list of items \b and sorts them
         * @param list - List of items
         */
        heap(std::initializer_list<T> list) : buffer(list.size()) {
            for (auto i: list) {
                this->push(i);
            }
        }

        /**
         * Constructor; initializes the heap with list of items \b and sorts them with a custom sorting condition
         * @tparam FuncSig - Implicit return of function
         * @param list - List of items
         * @param f - Custom sorting condition
         */
        template<class FuncSig>
        heap(std::initializer_list<T> list, FuncSig f) : buffer(list.size()), condition(f) {
            for (auto i: list) {
                this->push(i);
            }
        }

        /**
         * Destructs buffer
         */
        ~heap() {
            buffer.~vector();
            condition = nullptr;
        }

        /**
         * Clears the buffer, fills it with the list of items \b and sorts them
         * @param list - List of items
         * @return Self
         */
        heap<T> &operator=(std::initializer_list<T> list) {
            buffer.clear();

            for (auto i: list) {
                this->push(i);
            }

            return *this;
        }

        // ***************
        // * Item Access *
        // ***************
        /**
         * Returns reference to base item
         * @return Item
         */
        T &peek() { return buffer[0]; }

        /**
         * Returns const reference to base item
         * @return Item
         */
        T &peek() const { return buffer[0]; }

        // ************
        // * Capacity *
        // ************
        bool empty() noexcept { return buffer.empty(); }

        size_t size() noexcept { return buffer.size(); }

        size_t capacity() const noexcept { return buffer.capacity(); }

        size_t max_capacity() const noexcept { return buffer.max_capacity(); }

        // *************
        // * Modifiers *
        // *************
        /**
         * Clears the heaps buffer
         */
        void clear() {
            buffer.clear();
        }

        /**
         * Copies an item onto the heap and sorts it
         * @param item - Item to be copied
         */
        void push(const T &item) {
            buffer.push_back(item);

            this->_internal_push();
        }

        /**
         * Moves an item onto the heap and sorts it
         * @param item - Item to be moved
         */
        void push(T &&item) {
            buffer.push_back(std::move(item));

            this->_internal_push();
        }

        /**
         * Emplaces an item in the heap and sorts it
         * @tparam Args - Unknown amount of arguments and their types
         * @param args - Argument(s) for the constructor
         */
        template<class... Args>
        void emplace(Args &&... args) {
            buffer.emplace_back(std::move(args)...);

            this->_internal_push();
        }

        /**
         * Removes the base item and rearranges the remaining heap
         */
        void pop() {
            buffer.swap_items(0, buffer.size() - 1);
            buffer.pop_back();

            this->_internal_remove();
        }
    };
}

#endif

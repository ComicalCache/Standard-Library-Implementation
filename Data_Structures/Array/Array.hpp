#ifndef EXT_ARRAY_HPP
#define EXT_ARRAY_HPP

#include <utility>
#include <stdexcept>

#define EXT_ARRAY_ASSERT_INDEX(i)  if (sizeof...(i) != this->_d) { throw std::runtime_error("indices of wrong dimension"); }
#define EXT_ARRAY_ASSERT_DIMENSION_COUNT(n) if (n > this->_d) { throw std::runtime_error("too many dimension sizes"); }

#define EXT_ARRAY_INTERNAL_CLEAR_ITEMS_RETURN(b) typename std::enable_if<std::is_trivially_destructible<X>::value == b>::type

#define EXT_ARRAY_BUFFER_INIT(size) (T *) ::operator new(size * sizeof(T))
#define EXT_ARRAY_DIMENSION_SIZE_INIT(N) (size_t *) ::operator new(N * sizeof(size_t))

namespace ext {
    /**
     * A D dimensional static sized array where each dimension is of length N storing T's. <br>
     * N and D have a default value of 1 for ease of usage in some cases.
     * @tparam T - Type to store
     * @tparam D - Number of dimensions
     * @tparam N - Size of each dimension
     */
    template<class T, size_t D = 1, size_t... N>
    class array {
    private:
        /**
         * Length of dimensions
         */
        size_t *_n;
        /**
         * Number of dimensions
         */
        size_t _d;
        size_t _size;
        T *buffer;

    private:
        /**
         * Calculates the index in the one-dimensional storage
         * @tparam Args
         * @param indices - i_1, ..., i_n
         * @return The "flattened" index
         */
        template<typename... Args>
        size_t _internal_calculate_index(Args... indices) {
            size_t index = 0;
            size_t init_iter = true;

            size_t count = 1;
            for (const auto i: {indices...}) {
                if (!init_iter) {
                    index *= this->_n[count];
                    index += i;
                    count += 1;
                } else {
                    index = i;
                    init_iter = false;
                    continue;
                }
            }

            return index;
        };

        template<typename... Ns>
        void _internal_constructor_work(size_t d, Ns... ns) {
            this->_n = EXT_ARRAY_DIMENSION_SIZE_INIT(d);
            this->_d = d;

            size_t supplied_dims = sizeof...(Ns);
            EXT_ARRAY_ASSERT_DIMENSION_COUNT(supplied_dims)

            size_t index = 0;
            for (const auto i: {ns...}) {
                this->_n[index] = i;
                index += 1;
            }
            // fill sizes with last known size
            if (supplied_dims < this->_d) {
                index -= 1; // adjust to last entry
                for (size_t i = index + 1; i < this->_d; i += 1) {
                    this->_n[i] = this->_n[index];
                }
            }

            this->_size = this->_n[0];
            for (size_t i = 1; i < this->_d; i += 1) {
                this->_size *= this->_n[i];
            }
            this->buffer = EXT_ARRAY_BUFFER_INIT(this->_size);
        };

        template<class X>
        EXT_ARRAY_INTERNAL_CLEAR_ITEMS_RETURN(false) _internal_clear_items() {
        };

        template<class X>
        EXT_ARRAY_INTERNAL_CLEAR_ITEMS_RETURN(true) _internal_clear_items() {
        };

    public:
        array() {
            this->_internal_constructor_work(D, N...);
        };

        template<typename... Ns>
        array<T, 0, 0>(size_t d, Ns... ns) {
            this->_internal_constructor_work(d, ns...);
        };

        ~array() {
            if (this->buffer != nullptr) {
                this->_internal_clear_items<T>();
                ::operator delete(buffer);
                this->buffer = nullptr;
            }
        };

        // ***************
        // * Item Access *
        // ***************
        template<typename... Args>
        T &operator[](Args... indices) {
            return this->buffer[this->_internal_calculate_index(indices...)];
        };

        template<typename... Args>
        const T &operator[](Args... indices) const {
            return this->buffer[this->_internal_calculate_index(indices...)];

        };

        // ************
        // * Capacity *
        // ************
        size_t dimensions() {
            return this->_d;
        };

        size_t length_of_dimension(size_t dim) {
            // TODO: change this
            if (dim > this->_d || dim < 1) {
                throw std::runtime_error("invalid index");
            }

            return this->_n[dim - 1];
        };

        size_t size() {
            return this->_size;
        };
    };
}

#endif
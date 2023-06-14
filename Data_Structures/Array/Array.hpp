#ifndef EXT_ARRAY_HPP
#define EXT_ARRAY_HPP

#include <utility>
#include <stdexcept>

#define EXT_ARRAY_ASSERT_INDEX(i)  if (i >= this->_dsize) { throw std::runtime_error("val out of range"); }
#define EXT_ARRAY_ASSERT_DIMENSION_COUNT_SIZES(n) if (n > this->_d) { throw std::runtime_error("too many dimension sizes"); }
#define EXT_ARRAY_ASSERT_DIMENSION_COUNT(n) if (n < 1) { throw std::runtime_error("too few dimensions"); }
#define EXT_ARRAY_ASSERT_DIMENSION_SIZE(n) if (n < 1) { throw std::runtime_error("dimension size must be bigger than 0"); }

#define EXT_ARRAY_TRIVIAL_DESTRUCTIBLE(T, b) (std::is_trivially_destructible<T>::value == b)

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
            EXT_ARRAY_ASSERT_DIMENSION_COUNT(d);

            this->_n = EXT_ARRAY_DIMENSION_SIZE_INIT(d);
            this->_d = d;

            size_t supplied_dims = sizeof...(Ns);
            EXT_ARRAY_ASSERT_DIMENSION_COUNT_SIZES(supplied_dims)

            size_t index = 0;
            for (const auto i: {ns...}) {
                EXT_ARRAY_ASSERT_DIMENSION_SIZE(i)

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

        void _internal_clear_items() requires EXT_ARRAY_TRIVIAL_DESTRUCTIBLE(T, false) {
            // TODO
        };

        void _internal_clear_items() requires EXT_ARRAY_TRIVIAL_DESTRUCTIBLE(T, true) {
            // TODO
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
                this->_internal_clear_items();
                ::operator delete(this->buffer);
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

        template<typename... Args>
        T &at(Args... indices) {
            size_t index = this->_internal_calculate_index(indices...);
            EXT_ARRAY_ASSERT_INDEX(index)

            return this->buffer[index];
        };

        template<typename... Args>
        const T &at(Args... indices) const {
            size_t index = this->_internal_calculate_index(indices...);
            EXT_ARRAY_ASSERT_INDEX(index)

            return this->buffer[index];

        };

        // ************
        // * Capacity *
        // ************
        size_t dimensions() {
            return this->_d;
        };

        size_t length_of_dimension(size_t dim) {
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
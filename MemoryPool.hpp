
#ifdef _MSC_VER
#pragma once
#endif
#ifndef HTCW_MEMORYPOOL
#define HTCW_MEMORYPOOL
#include <cinttypes>
#include <cstddef>
using namespace std;
namespace mem {
    // represents an interface/contract for a memory pool
    struct MemoryPool {
    public:
        // allocates the specified number of bytes
        // returns nullptr if there's not enough free
        virtual void* alloc(const size_t size)=0;
        // invalidates all the pointers in the pool and frees the memory
        virtual void freeAll()=0;
        // retrieves the next pointer that will be allocated
        // (for optimization opportunities)
        virtual void* next()=0;
        // indicates the maximum capacity of the pool
        virtual size_t capacity()=0;
        // indicates how many bytes are currently used
        virtual size_t used()=0;
    };

    // represents a memory pool whose maximum capacity is known at compile time
    template<size_t C> class StaticMemoryPool : public MemoryPool {
        // the actual buffer
        uint8_t _heap[C];
        // the next free pointer
        uint8_t *_next;
    public:
        // allocates the specified number of bytes
        // returns nullptr if there's not enough free
        void* alloc(const size_t size) override {
            // if we don't have enough room return null
            if(used()+size>=C)
                return nullptr;
            // get our pointer and reserve the space
            void* result = _next;
            _next+=size;
            // return it
            return result;
        }
        // invalidates all the pointers in the pool and frees the memory
        void freeAll() override {
            // just set next to the beginning
            _next = _heap;
        }
        // retrieves the next pointer that will be allocated
        // (for optimization opportunities)
        void *next() override {
            if(!C)
                return nullptr;
            return _next;
        }
        // indicates the maximum capacity of the pool
        size_t capacity() override { return C; }
        // indicates how many bytes are currently used
        size_t used() override {return _next-_heap;}
        StaticMemoryPool() : _next(_heap) {}
        ~StaticMemoryPool() {}
    };
    
    // represents a memory pool whose maximum capacity is determined at runtime
    class DynamicMemoryPool : public MemoryPool {
        // the actual buffer
        uint8_t *_heap;
        // the capacity
        size_t _capacity;
        // the next free pointer
        uint8_t *_next;
    public:
        // initializes the dynamic pool with the specified capacity
        DynamicMemoryPool(const size_t capacity) {
            // special case for 0 cap pool
            if(0==_capacity) {
                _heap=_next = nullptr;
            }
            // reserve space from the heap
            _heap = new uint8_t[capacity];
            // initialize the next pointer
            _next=_heap;
            if(nullptr==_heap)
                _capacity=0;
        }
        // allocates the specified number of bytes
        // returns nullptr if there's not enough free
        void* alloc(const size_t size) override {
            if(nullptr==_heap)
                return nullptr;
            // if we don't have enough room, return null
            if(used()+size>=_capacity) {
                return nullptr;
            }
            // store the result pointer, then increment next
            // reserving space
            void* result = _next;
            _next+=size;
            // return it
            return result;
        }
        // invalidates all the pointers in the pool and frees the memory
        void freeAll() override {
            // just set next to the beginning
            _next = _heap;
        }
        // retrieves the next pointer that will be allocated
        // (for optimization opportunities)
        void* next() override {
            // just return the next pointer
            return _next;
        }
        // indicates the maximum capacity of the pool
        size_t capacity() override { if(nullptr==_heap) return 0; return _capacity; }
        // indicates how many bytes are currently used
        size_t used() override { return _next-_heap;}
        ~DynamicMemoryPool() { if(nullptr!=_heap) delete _heap;}
    };
}
#endif

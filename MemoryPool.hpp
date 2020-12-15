
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
    template<size_t TCapacity> class StaticMemoryPool : public MemoryPool {
        // the actual buffer
        uint8_t m_heap[TCapacity];
        // the next free pointer
        uint8_t *m_next;
    public:
        // allocates the specified number of bytes
        // returns nullptr if there's not enough free
        void* alloc(const size_t size) override {
            // if we don't have enough room return null
            if(used()+size>TCapacity)
                return nullptr;
            // get our pointer and reserve the space
            void* result = m_next;
            m_next+=size;
            // return it
            return result;
        }
        // invalidates all the pointers in the pool and frees the memory
        void freeAll() override {
            // just set next to the beginning
            m_next = m_heap;
        }
        // retrieves the next pointer that will be allocated
        // (for optimization opportunities)
        void *next() override {
            if(!TCapacity)
                return nullptr;
            return m_next;
        }
        // indicates the maximum capacity of the pool
        size_t capacity() override { return TCapacity; }
        // indicates how many bytes are currently used
        size_t used() override {return m_next-m_heap;}
        StaticMemoryPool() : m_next(m_heap) {}
        ~StaticMemoryPool() {}
    };
    
    // represents a memory pool whose maximum capacity is determined at runtime
    class DynamicMemoryPool : public MemoryPool {
        // the actual buffer
        uint8_t *m_heap;
        // the capacity
        size_t m_capacity;
        // the next free pointer
        uint8_t *m_next;
    public:
        // initializes the dynamic pool with the specified capacity
        DynamicMemoryPool(const size_t capacity) {
            // special case for 0 cap pool
            if(0==m_capacity) {
                m_heap=m_next = nullptr;
            }
            // reserve space from the heap
            m_heap = new uint8_t[capacity];
            // initialize the next pointer
            m_next=m_heap;
            if(nullptr==m_heap)
                m_capacity=0;
        }
        // allocates the specified number of bytes
        // returns nullptr if there's not enough free
        void* alloc(const size_t size) override {
            if(nullptr==m_heap)
                return nullptr;
            // if we don't have enough room, return null
            if(used()+size>m_capacity) {
                return nullptr;
            }
            // store the result pointer, then increment next
            // reserving space
            void* result = m_next;
            m_next+=size;
            // return it
            return result;
        }
        // invalidates all the pointers in the pool and frees the memory
        void freeAll() override {
            // just set next to the beginning
            m_next = m_heap;
        }
        // retrieves the next pointer that will be allocated
        // (for optimization opportunities)
        void* next() override {
            // just return the next pointer
            return m_next;
        }
        // indicates the maximum capacity of the pool
        size_t capacity() override { if(nullptr==m_heap) return 0; return m_capacity; }
        // indicates how many bytes are currently used
        size_t used() override { return m_next-m_heap;}
        ~DynamicMemoryPool() { if(nullptr!=m_heap) delete m_heap;}
    };
}
#endif

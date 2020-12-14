
#ifdef _MSC_VER
#pragma once
#endif
#ifndef HTCW_MEMORYPOOL
#define HTCW_MEMORYPOOL
#include <cinttypes>
#include <cstddef>
using namespace std;
namespace pool {
    struct MemoryPool {
    public:
        virtual void* alloc(const size_t size)=0;
        virtual void freeAll()=0;
        virtual void* next()=0;
        virtual size_t capacity()=0;
        virtual size_t used()=0;
    };
    template<size_t C> class StaticMemoryPool : public MemoryPool {
        uint8_t _heap[C];
        uint8_t *_next;
    public:
        void* alloc(const size_t size) override {
            if(used()+size>=C)
                return nullptr;
            void* result = _next;
            _next+=size;
            return result;
        }
        void freeAll() override {
            _next = _heap;
        }
        void *next() override {
            return _next;
        }
        size_t capacity() override { return C; }
        size_t used() override {return _next-_heap;}
        StaticMemoryPool() : _next(_heap) {}
        ~StaticMemoryPool() {}
    };
    class DynamicMemoryPool : public MemoryPool {
        uint8_t *_heap;
        size_t _capacity;
        uint8_t *_next;
        DynamicMemoryPool(const size_t capacity) {
            _heap = new uint8_t[capacity];
            _next=_heap;
        }
        void* alloc(const size_t size) override {
            if(nullptr==_heap)
                return nullptr;
            if(used()+size>=_capacity) {
                return nullptr;
            }
            void* result = _next;
            _next+=size;
            return result;
        }
        void freeAll() override {
            _next = _heap;
        }
        void* next() override {
            return _next;
        }
        size_t capacity() override { if(nullptr==_heap) return 0; return _capacity; }
        size_t used() override { return _next-_heap;}
        ~DynamicMemoryPool() { if(nullptr!=_heap) delete _heap;}
    };
}
#endif
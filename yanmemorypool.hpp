#pragma once
#include <cassert>
#include <cstddef>
#include <mutex>
#include <new>
#include <utility>
 namespace Memorypool{
 #define MAX_SLOT_MEMORY 512
 #define  SLOT_MEMORY 8
 #define MAX_MEMORY 64
struct Slot
{
    Slot*next;
};
class memorypool
{     public:
    explicit memorypool(size_t BlockSize_ = 4096);
    ~memorypool();
    void init(size_t);
    void *alloc();
    void delalloc(void* p, size_t n);
    private:
    void allocatenewBlock();//向系统申请内存
    size_t pad(char*p,size_t align);//将内存对齐
    int BlockSize_;//槽的大小
    int SlotSize_;//块的大小
    Slot*freeSlot;//已经使用但是已经回收了的内存
    Slot*curSlot;
    Slot*firstBlock;
    Slot*lastSlot;
    std:: mutex mutexforfreeslot;
    std:: mutex mutexforBlock;
};
class HashBucket
{  
    public:
    static void initmemorypool();
    static memorypool&getmemorypool(int idex);
   
   static  void*usememory(size_t p)
     {
        static const bool initialized = (initmemorypool(), true);
        (void)initialized;
        if(p==0)
        {
            return nullptr;
        }
        if(p>MAX_SLOT_MEMORY)
        {
            return ::operator new(p);
        }
        return getmemorypool((p + SLOT_MEMORY - 1) / SLOT_MEMORY - 1).alloc();
     }
   static  void freememory(size_t p,Slot*ptr)
     {
        static const bool initialized = (initmemorypool(), true);
        (void)initialized;
        if(!ptr)
        {
            return;
        }
        if(p>MAX_SLOT_MEMORY)
        {
            ::operator delete(ptr);
            return;
        }
        getmemorypool((p + SLOT_MEMORY - 1) / SLOT_MEMORY - 1).delalloc(ptr, p);
     }
     template<typename T ,typename...  Args>
     friend T*newElement(Args&&...args);
     template<typename T>
     friend void deleteElement(T*ptr);
};
 template<typename T ,typename...  Args>
    T*newElement(Args&&...args)
    {
        T*p=nullptr;
        if((p=reinterpret_cast<T*>(HashBucket::usememory(sizeof(T))))!=nullptr)
        {
             new (p)T(std::forward<Args>(args)...);

        }
        return p;
    }
      template<typename T>
     void deleteElement(T*ptr)
     {
        if(ptr)
        {
            ptr->~T();
            HashBucket::freememory(sizeof(T),reinterpret_cast<Slot*>(ptr));
        }
     }
}

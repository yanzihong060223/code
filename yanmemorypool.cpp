#include"yanmemorypool.hpp"
 namespace Memorypool
{
    memorypool::memorypool(size_t BlockSize)
       : BlockSize_(BlockSize)
    , SlotSize_(0)
    , freeSlot(nullptr)
    , curSlot(nullptr)
    , firstBlock(nullptr)
    , lastSlot(nullptr)
{}
   void  memorypool::init(size_t size)//内存池初始化
    {
        assert(size>0);
        SlotSize_=size;
    freeSlot=nullptr;
    curSlot=nullptr;
    firstBlock=nullptr;
    lastSlot=nullptr;
    }
    memorypool::~memorypool()
    {
        while(firstBlock!=nullptr)
        {
            Slot*idex=firstBlock->next;
            ::operator delete(firstBlock);
            firstBlock=idex;
        }
    }
      void * memorypool::alloc()
      {
        if(freeSlot!=nullptr)//若freeslot中 还有空闲的内存 则先使用
        {
            std::lock_guard<std::mutex>lock(mutexforfreeslot);
            {
                if(freeSlot!=nullptr)
                {
                        Slot*temp=freeSlot;
                        freeSlot=freeSlot->next;
                        return temp;

                }
            }
        }
        Slot*temp;
        {
              std::lock_guard<std::mutex>lock(mutexforBlock);
              if(curSlot>=lastSlot)
              {
                allocatenewBlock();
              }
              temp=curSlot;
              curSlot+= SlotSize_;
        }
        return temp;
      }
       void memorypool::delalloc(void *p, size_t n)
       {
         (void)n;
         if(p)
         
         {
            std::lock_guard<std::mutex> lock(mutexforfreeslot);
            reinterpret_cast<Slot *>(p)->next=freeSlot;
            freeSlot= reinterpret_cast<Slot *>(p);
         }
       }
        void memorypool::allocatenewBlock()
        {
            void*newblock=::operator new(BlockSize_);
            reinterpret_cast<Slot*>(newblock)->next=firstBlock;
            firstBlock= reinterpret_cast<Slot*>(newblock);
            char *body=reinterpret_cast<char*>(newblock)+sizeof(Slot*);
            size_t pa=pad(body, SlotSize_);
            curSlot=reinterpret_cast<Slot*>(body+pa);
            lastSlot=reinterpret_cast<Slot*>(reinterpret_cast<char*>(newblock)+BlockSize_-SlotSize_+1);
        }
         size_t memorypool::pad(char*p,size_t align)
         {
            return(align-reinterpret_cast<size_t>(p))%align;
         }
           void HashBucket::initmemorypool()
           {
            for(int i=0;i<MAX_MEMORY;i++)
            {
                getmemorypool(i).init((i+1)*SLOT_MEMORY);
            }
           }
            memorypool& HashBucket::getmemorypool(int idex)
            {
                assert(idex >= 0 && idex < MAX_MEMORY);
                static memorypool mpl[MAX_MEMORY];
                return mpl[idex];
            }
}

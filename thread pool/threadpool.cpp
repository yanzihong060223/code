#include"threadpool.hpp"
threadpool ::threadpool(int size):
poolsize(size),
stop(false)
{
    for(int i=0;i<size;i++)
    {  
        threads.emplace_back([this](){
                    while(1)
                    {
                        std::unique_lock<std::mutex>lock(mutexforthread);
                        condition.wait(lock,[this](){
                            return !tasks.empty()||stop;
                        });
                        if(stop&&tasks.empty())
                        {
                            return ;
                        }
                        std::function<void()>task(std::move(tasks.front()));
                           
                        tasks.pop();
                         lock . unlock();
                        task();
                    }
        });
       
    }
}
threadpool:: ~threadpool()
{
    stop=true;
    condition.notify_all();
    for(auto&a:threads)
    {
        a.join();
    }
    
}
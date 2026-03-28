#pragma once
#include<thread>
#include<vector>
#include<queue>
#include<functional>
#include<mutex>
#include<atomic>
#include<condition_variable>

class threadpool
{   
   
    public:
  threadpool(int poolsize);
  
  
     
     ~threadpool();
   template<typename T,typename ...Args>
void enque(T&&t,Args&&...args)
{
    std::function<void()>task(std::bind(std::forward<T>(t),std::forward<Args>(args)...));
    {
        std::unique_lock<std::mutex>lock(mutexforthread);
         tasks.emplace(std::move(task));


        
    }
    condition.notify_one();
}

      private:
      
    int poolsize;//线程池中线程的数量
    std:: atomic< bool> stop;//判断线程池是否停止
    std:: vector<std::thread>threads;
    std:: queue<std::function<void()>>tasks;//任务队列
    std:: mutex mutexforthread;
    std:: condition_variable condition;
    
};

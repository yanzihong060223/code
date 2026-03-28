#include<iostream>
#include<ctime>
#include"threadpool.hpp"
#include<mutex>
std::mutex mutexforcount;
int main()
{
    threadpool t(3);
   
    for(int i=0;i<20;i++)
    {
         t.enque([i](){
            std::lock_guard<std::mutex>lock(mutexforcount);
           std:: cout<<"任务"<<i+1<<"正在运行"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
           std:: cout<<"任务"<<i+1<<"运行完成"<<std::endl;

         });
    }
    return 0;
}
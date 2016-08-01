#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
namespace ms{
    namespace work{

        template<class task>
        class work
        {
        public:
            work() : _stop(true) { ; }
            ~work()
            {
                WorkStop();
            }

            void WorkStart()
            {
                if (_stop)
                {
                    _stop = false;
                    _threadptr = std::make_shared<std::thread>(std::bind(&work::WorkRun, this));
                }
            }
            void WorkStop()
            {
                _stop = true;
                _cond.notify_all();
                _threadptr->join();
                std::lock_guard<std::mutex> guard(_mutex);
                while (!_taskqueue.empty())
                {
                    delete _taskqueue.front();
                    _taskqueue.pop();
                }
            }
            void PushTask(task* task)
            {
                std::lock_guard<std::mutex> guard(_mutex);
                _taskqueue.push(task);
                _cond.notify_one();
            }
            bool QueueIsEmpty()
            {
                std::lock_guard<std::mutex> guard(_mutex);
                return _taskqueue.empty();
            }
        private:
            bool _stop;
            std::mutex _mutex;
            std::mutex _threadwait;
            std::condition_variable _cond;
            std::queue<task*> _taskqueue;
            std::shared_ptr<std::thread> _threadptr;

            void WorkRun()
            {
                while (!_stop)
                {
                    task* ptask = NULL;
                    {
                        if (QueueIsEmpty())
                        {
                            std::unique_lock <std::mutex> lck(_threadwait);
                            _cond.wait(lck);
                            continue;
                        }
                        else
                        {
                            std::lock_guard<std::mutex> guard(_mutex);
                            ptask = _taskqueue.front();
                            _taskqueue.pop();
                        }
                    }
                    (*ptask)();
                    delete ptask;
                }
            }
        };

    }
}
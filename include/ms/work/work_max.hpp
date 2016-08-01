#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
namespace ms{
    namespace work{

        template<class task>
        class work_max
        {
        public:
            work_max() : _stop(true) { ; }
            ~work_max()
            {
                WorkStop();
            }

            void WorkStart()
            {
                if (_stop)
                {
                    _stop = false;
                    _threadptr = std::make_shared<std::thread>(std::bind(&work_max::WorkRun, this));
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
            void SetQueueMax(size_t max)
            {
                task_max_ = max;
            }
        private:
            size_t task_max_{ 1000 };
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
                            //_taskqueue.pop();
                        }
                    }
                    if ((*ptask)())
                    {
                        std::lock_guard<std::mutex> guard(_mutex);
                        _taskqueue.pop();
                        delete ptask;
                    }
                    else
                    {
                        std::lock_guard<std::mutex> guard(_mutex);
                        while (_taskqueue.size () > task_max_)
                        {
                            delete _taskqueue.front();
                            _taskqueue.pop();
                        }
                    }
                }
            }
        };

    }
}
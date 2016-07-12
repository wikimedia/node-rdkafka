#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include<vector>
#include<uv.h>

template <class JOB_TYPE> class JobQueue {
    public:
        JobQueue() {
            uv_mutex_init(&this->mutex);
            uv_cond_init(&this->cond);
        }
        ~JobQueue() {
            uv_mutex_destroy(&this->mutex);
            uv_cond_destroy(&this->cond);
        }

        void pushJob(JOB_TYPE* job) {
            uv_mutex_lock(&this->mutex);
            {
                this->jobRequests.push_back(job);
            }
            uv_cond_signal(&this->cond);
            uv_mutex_unlock(&this->mutex);
        }

        std::vector<JOB_TYPE*> pullJobs() {
            std::vector<JOB_TYPE*> result;
            uv_mutex_lock(&this->mutex);
            {
                while(this->jobRequests.size() == 0) {
                    uv_cond_wait(&this->cond, &this->mutex);
                }

                std::copy(this->jobRequests.begin(), this->jobRequests.end(), std::back_inserter(result));
                this->jobRequests.clear();
            }
            uv_mutex_unlock(&this->mutex);
            return result;
        }
    private:
        std::vector<JOB_TYPE*> jobRequests;
        uv_mutex_t mutex;
        uv_cond_t cond;
};

#endif
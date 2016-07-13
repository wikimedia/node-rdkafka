#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include<vector>
#include<uv.h>

template <class JOB_TYPE, class RESULT_TYPE> class JobQueue {
    public:
        JobQueue() {
            this->jobRequests = new std::vector<JOB_TYPE*>();
            this->jobResults = new std::vector<RESULT_TYPE*>();
            uv_mutex_init(&this->jobRequestMutex);
            uv_mutex_init(&this->resultMutex);
            uv_cond_init(&this->cond);
        }
        ~JobQueue() {
            uv_mutex_destroy(&this->jobRequestMutex);
            uv_mutex_destroy(&this->resultMutex);
            uv_cond_destroy(&this->cond);
        }

        void pushJob(JOB_TYPE* job) {
            uv_mutex_lock(&this->jobRequestMutex);
            {
                this->jobRequests->push_back(job);
            }
            uv_cond_signal(&this->cond);
            uv_mutex_unlock(&this->jobRequestMutex);
        }

        void pushResult(RESULT_TYPE* result) {
            uv_mutex_lock(&this->resultMutex);
            {
                this->jobResults->push_back(result);
            }
            uv_mutex_unlock(&this->resultMutex);
        }

        std::vector<JOB_TYPE*>* pullJobs() {
            std::vector<JOB_TYPE*>* result;
            uv_mutex_lock(&this->jobRequestMutex);
            {
                result = new std::vector<JOB_TYPE*>(this->jobRequests->begin(), this->jobRequests->end());
                delete this->jobRequests;
                this->jobRequests = new std::vector<JOB_TYPE*>();
            }
            uv_mutex_unlock(&this->jobRequestMutex);
            return result;
        }

        std::vector<RESULT_TYPE*>* pullResults() {
            std::vector<RESULT_TYPE*>* result;
            uv_mutex_lock(&this->resultMutex);
            {
                result = new std::vector<RESULT_TYPE*>(this->jobResults->begin(), this->jobResults->end());
                delete this->jobResults;
                this->jobResults = new std::vector<RESULT_TYPE*>();
            }
            uv_mutex_unlock(&this->resultMutex);
            return result;
        }
    private:
        std::vector<JOB_TYPE*>* jobRequests;
        std::vector<RESULT_TYPE*>* jobResults;
        uv_mutex_t jobRequestMutex;
        uv_mutex_t resultMutex;
        uv_cond_t cond;
};

#endif
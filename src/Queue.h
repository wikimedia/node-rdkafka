#ifndef content_QUEUE_H
#define content_QUEUE_H

#include<vector>
#include<uv.h>

template <class CONTENT_TYPE> class Queue {
    public:
        Queue(bool isBlocking) {
            blocking = isBlocking;
            running = true;
            this->contentVector = new std::vector<CONTENT_TYPE*>();
            uv_mutex_init(&this->mutex);
            uv_cond_init(&this->cond);
        }
        ~Queue() {
            uv_cond_destroy(&this->cond);
            uv_mutex_destroy(&this->mutex);
            if (this->contentVector) {
                delete this->contentVector;
            }
        }

        void push(CONTENT_TYPE* content) {
            uv_mutex_lock(&this->mutex);
            {
                this->contentVector->push_back(content);
            }
            uv_cond_signal(&this->cond);
            uv_mutex_unlock(&this->mutex);
        }

        std::vector<CONTENT_TYPE*>* pull() {
            std::vector<CONTENT_TYPE*>* result;
            uv_mutex_lock(&this->mutex);
            {
                if (!this->running) {
                    uv_mutex_unlock(&this->mutex);
                    return NULL;
                }

                if (this->blocking) {
                    while(this->contentVector->size() == 0
                        && uv_cond_timedwait(&this->cond, &this->mutex, 500) != 0) {
                        if (!this->running) {
                            uv_mutex_unlock(&this->mutex);
                            return NULL;
                        }
                    }
                }

                result = new std::vector<CONTENT_TYPE*>(this->contentVector->begin(), this->contentVector->end());
                delete this->contentVector;
                this->contentVector = new std::vector<CONTENT_TYPE*>();
            }
            uv_mutex_unlock(&this->mutex);
            return result;
        }

        void stop() {
            uv_mutex_lock(&this->mutex);
            this->running = false;
            uv_mutex_unlock(&this->mutex);
        }
    private:
        std::vector<CONTENT_TYPE*>* contentVector;
        uv_mutex_t mutex;
        uv_cond_t cond;

        // TODO: make it constant
        bool blocking;
        bool running;
};

#endif
#include <queue>
#include <thread>
#include <mutex>

template <typename T>
class Concurrent_Queue
{
 public:

  T pop()
  {

    bool bSuccess = m_mMutex.try_lock();

    if (!bSuccess || m_qQueue.empty())
    {
      return nullptr;
    }

    auto item = m_qQueue.front();
    m_qQueue.pop();

    m_mMutex.unlock();
    return item;
  }

  void push(T& item)
  {

    m_mMutex.lock();
    m_qQueue.push(item);
    m_mMutex.unlock();
  }

  bool isEmpty()
  {
	m_mMutex.lock();
	bool bIsEmpty = m_qQueue.empty();
	m_mMutex.unlock();
	return bIsEmpty;
  }


 private:
  std::queue<T> m_qQueue;
  std::mutex m_mMutex;
};

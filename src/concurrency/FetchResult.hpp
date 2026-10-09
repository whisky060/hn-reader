#ifndef FETCHRESULT_H
#define FETCHRESULT_H

#include <optional>
#include <exception>
#include <stdexcept>

template<class T>
class FetchResult
{

  public:
    FetchResult(T value) : optionalValue_(value){};
    FetchResult(std::exception_ptr exception) : exception_(exception) {};
    ~FetchResult() = default;
    bool OK() const noexcept { return optionalValue_.has_value(); };
    static FetchResult Success(T value){ return FetchResult(value);}
    static FetchResult Failure(std::exception_ptr exception){return FetchResult(exception);}
    std::exception_ptr GetException() { return exception_; };
    T GetValue()
    {
      if(this->OK())
        return  optionalValue_.value();

      throw std::runtime_error("accessed a value when it was't available");
    }

  private:
    std::optional<T> optionalValue_;
    std::exception_ptr exception_;
};

#endif

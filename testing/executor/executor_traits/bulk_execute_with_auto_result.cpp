#include <agency/agency.hpp>
#include <agency/execution/executor/detail/new_executor_traits.hpp>
#include <iostream>

#include "test_executors.hpp"


template<class Executor>
void test_returning_void(Executor exec)
{
  using namespace agency::detail::new_executor_traits_detail;

  agency::new_executor_shape_t<Executor> shape{100};
  
  int shared_arg = 0;
  
  int increment_me = 0;
  std::mutex mut;
  bulk_execute_with_auto_result(exec, [&](size_t idx, int& shared_arg)
  {
    mut.lock();
    increment_me += 1;
    ++shared_arg;
    mut.unlock();
  },
  shape,
  [&]
  {
    return std::ref(shared_arg);
  });
  
  assert(increment_me == shape);
  assert(shared_arg == shape);
}


template<class Executor>
void test_returning_results(Executor exec)
{
  using namespace agency::detail::new_executor_traits_detail;

  using shape_type = agency::new_executor_shape_t<Executor>;
  using index_type = agency::new_executor_index_t<Executor>;

  size_t shape = 10;
  
  auto result = bulk_execute_with_auto_result(exec,
    [](index_type idx, std::vector<int>& shared_arg)
    {
      return shared_arg[idx];
    },
    shape,
    [=]{ return std::vector<int>(shape, 13); }  // shared_arg
  );
  
  using container_type = agency::new_executor_container_t<Executor,int>;
  assert(container_type(shape, 13) == result);
}


int main()
{
  test_returning_void(bulk_synchronous_executor());
  test_returning_void(bulk_asynchronous_executor());
  test_returning_void(bulk_continuation_executor());
  test_returning_void(not_a_bulk_synchronous_executor());
  test_returning_void(not_a_bulk_asynchronous_executor());
  test_returning_void(not_a_bulk_continuation_executor());
  test_returning_void(complete_bulk_executor());

  test_returning_results(bulk_synchronous_executor());
  test_returning_results(bulk_asynchronous_executor());
  test_returning_results(bulk_continuation_executor());
  test_returning_results(not_a_bulk_synchronous_executor());
  test_returning_results(not_a_bulk_asynchronous_executor());
  test_returning_results(not_a_bulk_continuation_executor());
  test_returning_results(complete_bulk_executor());

  std::cout << "OK" << std::endl;
  
  return 0;
}


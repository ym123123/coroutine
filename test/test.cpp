//============================================================================
// Name        : test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stack>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <ucontext.h>

using namespace std;

class context
{
public:
  context()
  {
    getcontext(this->get_context());
    this->base.uc_stack.ss_sp = this->buf;
    this->base.uc_stack.ss_size = 1024;
    this->prev = this->next = NULL;
    this->data = NULL;
  }
  void set_prev(ucontext_t *prev)
  {
    this->prev = prev;
  }

  ucontext_t *get_prev()
  {
    return this->prev;
  }

  void set_next(ucontext_t *next)
  {
    this->next = next;
  }

  ucontext_t *get_next()
  {
    return this->next;
  }

  void *get_data()
  {
    return this->data;
  }

  void set_data(void *data)
  {
    this->data = data;
  }

  ucontext_t *get_context()
  {
    return &this->base;
  }

  void *swap_func(context *n)
  {
    void *ret;
    this->set_next(n->get_context());
    n->set_prev(this->get_context());

    while (!n->get_data())
    {
      swapcontext(this->get_context(), n->get_context());

      if (n->get_data())
      {
        break;
      }
      if (this->get_prev() == NULL)
        return NULL;
      swapcontext(this->get_context(), this->get_prev());
    }


    ret = n->get_data();
    this->set_data(ret);
    free(n);
    swapcontext(this->get_context(), this->get_prev());
    return ret;
  }
private:
  ucontext_t base;
  char buf[1024];
  ucontext_t *prev;
  ucontext_t *next;
  void *data;
};

context *ctx = NULL;

void test1(context *ctx)
{
  int i = 10;
  while (i--)
  {
    cout << "====" << i << "====" << endl;
    swapcontext(ctx->get_context(), ctx->get_prev());
  }

  ctx->set_data((void *)"yangmeng");
  swapcontext(ctx->get_context(), ctx->get_prev());
  return;
}

void *test()
{
  context base;
  getcontext(base.get_context());

  cout << "===================" << endl;
  if (ctx == NULL)
  {
    ctx = new context();
    makecontext(ctx->get_context(), (void (*)())test1, 1, ctx);
  }

  base.swap_func(ctx);

  return base.get_data();
}


int main()
{
  while (!test())
  {
    cout << "###############" << endl;
    sleep(1);
  }
}


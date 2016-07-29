#include <iostream>
#include <list>

#ifndef __PACK_H__
#define __PACK_H__

// Pack a pointer to be automatically deleted:
template<class T> class pack {
  // Reference Counter type:
  struct RC_t {
    int count;
    T* obj;
    bool _delete;
  };

  typedef std::list<RC_t> RefList_t;
  typedef typename RefList_t::iterator Ref_t;

 public:
  static RefList_t* rlist() {
    static RefList_t* list = new RefList_t();
    return list;
  }

 protected:
  // Reference counter:
  Ref_t ref;

 protected:

  void dropRef() {
    --(ref->count);

    // If the reference count gets to 0:
    if(ref->count == 0) {
      if (ref->_delete) {
        delete ref->obj;
      }
      rlist()->erase(ref);
    }
  }

  // Copy a reference from another
  // packed object:
  void getRef(const pack<T>& p) {
    ref = p.ref;
    ++(ref->count);
  }

  Ref_t build(T* val, bool _delete = true) {
    return rlist()->insert(rlist()->end(), {1, val, _delete});
  }

 public:
  friend void swap(pack<T>& first, pack<T>& second) {
    using std::swap;
    swap(first.ref, second.ref);
  }

 public:
  pack() : ref(build(new T())) {}
  pack(const T& t) : ref(build(new T(t))) {}
  pack(const pack<T>& p) { getRef(p); }
  pack(pack<T>&& p) : pack() { swap(*this, p); }
  virtual ~pack() { dropRef(); }

  // This constructor should not be used frequently
  // It won't delete the pointer, but will
  // update the reference counter anyway, so its inefficient.
  explicit pack(T* t) : ref(build(t, false)) {}

  // Copy assignment operator
  pack<T>& operator=(pack<T> p) {
    swap(*this, p);
    return *this;
  }

  operator T*() const { return ref->obj; }
  T* operator->() const { return ref->obj; }
  T& operator*() const { return *(ref->obj); }
  T* get() const { return ref->obj; }
};

#endif

#ifndef PROTON_THREAD_SAFE_HPP
#define PROTON_THREAD_SAFE_HPP

/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "./internal/config.hpp"
#include "./connection.hpp"
#include "./event_loop.hpp"
#include "./internal/object.hpp"
#include "./internal/type_traits.hpp"

#include <functional>

// FIXME aconway 2016-05-03: doc

namespace proton {

class connection;
class session;
class link;
class sender;
class receiver;

namespace internal {
template <class T> struct endpoint_traits;
template<> struct endpoint_traits<connection> {};
template<> struct endpoint_traits<session> {};
template<> struct endpoint_traits<link> {};
template<> struct endpoint_traits<sender> {};
template<> struct endpoint_traits<receiver> {};
}

template <class T> class returned;

// FIXME aconway 2016-05-09: doc
/// **Experimental** - A thread-safe object wrapper.
///    
/// Events for each proton::connection are processed sequentially in
/// an event_loop. proton::messaging_handler functions for a single connection
/// are never called concurrently. inject() lets you add user-defined
/// function calls to be processed in the event loop sequence.
///
/// thread_safe is useful with multi-threaded programs, where
/// different connection's event loops can run concurrently. Proton
/// objects associated with a connection (proton::connection,
/// proton::sender, etc.) are not thread safe, so they can only be
/// used in the context of the connection's thread_safe.  inject()
/// allows any thread (application threads or thread_safe threads for
/// different connections) to communicate safely.
template <class T>
class thread_safe : private internal::pn_ptr_base, private internal::endpoint_traits<T> {
    typedef typename T::pn_type pn_type;

    struct inject_decref : public inject_handler {
        pn_type* ptr_;
        inject_decref(pn_type* p) : ptr_(p) {}
        void on_inject() { decref(ptr_); delete this; }
    };

  public:
    /// @cond INTERNAL
    static void operator delete(void*) {}
    /// @endcond

    ~thread_safe() {
        if (ptr()) {
            if (event_loop()) {
#if PN_CPP_HAS_CPP11
                event_loop()->inject(std::bind(&decref, ptr()));
#else
                event_loop()->inject(*new inject_decref(ptr()));
#endif
            } else {
                decref(ptr());
            }
        }
    }

    /// Get the event loop for this object.
    class event_loop* event_loop() { return event_loop::get(ptr()); }

    /// @cond INTERNAL
    /// XXX Not sure what the status of these is
    
    // FIXME aconway 2016-05-04: doc
    T unsafe() { return T(ptr()); }

    // Caller must delete
    static thread_safe* create(const T& obj) { return new (obj.pn_object()) thread_safe(); }

    /// @endcond
    
  private:
    static void* operator new(size_t, pn_type* p) { return p; }
    static void operator delete(void*, pn_type*) {}
    thread_safe() { incref(ptr()); }
    pn_type* ptr() { return reinterpret_cast<pn_type*>(this); }

    // Non-copyable.
    thread_safe(const thread_safe&);
    thread_safe& operator=(const thread_safe&);

    /// @cond INTERNAL
  friend class returned<T>;
    /// @endcond
};

// FIXME aconway 2016-05-04: doc.
// Temporary return value only, not a real smart_ptr.
// Release or convert to some other pointer type immediately.
template <class T>
class returned : private internal::endpoint_traits<T>
{
  public:
    /// Take ownership
    explicit returned(thread_safe<T>* p) : ptr_(p) {}
    /// Transfer ownership.
    /// Use the same "cheat" as std::auto_ptr, calls x.release() even though x is const.
    returned(const returned& x) : ptr_(const_cast<returned&>(x).release()) {}
    /// Delete if still owned.
    ~returned() { if (ptr_) delete ptr_; }

    /// Release ownership.
    thread_safe<T>* release() const { thread_safe<T>* p = ptr_; ptr_ = 0; return p; }

    /// Implicit conversion to target, usable only in a safe context.
    operator T() { return ptr_->unsafe(); }

#if PN_CPP_HAS_CPP11
    /// Release to a std::shared_ptr
    operator std::shared_ptr<thread_safe<T> >() {
        return std::shared_ptr<thread_safe<T> >(release());
    }

    /// Release to a std::unique_ptr
    operator std::unique_ptr<thread_safe<T> >() {
        return std::unique_ptr<thread_safe<T> >(release());
    }
#endif

  private:
    void operator=(const returned&);
    mutable thread_safe<T>* ptr_;
};

// XXX Review this text
/// Make a thread-safe wrapper for `obj`.
template <class T> returned<T> make_thread_safe(const T& obj) {
    return returned<T>(thread_safe<T>::create(obj));
}

// XXX Review this text
/// Get a thread-unsafe pointer for `p`.
template <class T> T make_thread_unsafe(T* p) { return p->unsafe(); }

#if PN_CPP_HAS_CPP11
template <class T> std::shared_ptr<thread_safe<T> > make_shared_thread_safe(const T& obj) {
    return std::shared_ptr<thread_safe<T> >(thread_safe<T>::create(obj));
}
template <class T> std::unique_ptr<thread_safe<T> > make_unique_thread_safe(const T& obj) {
    return std::unique_ptr<thread_safe<T> >(thread_safe<T>::create(obj));
}

template <class T> T make_thread_unsafe(const std::shared_ptr<T>& p) { return p->unsafe(); }
template <class T> T make_thread_unsafe(const std::unique_ptr<T>& p) { return p->unsafe(); }
#endif

} // proton

#endif // PROTON_THREAD_SAFE_HPP

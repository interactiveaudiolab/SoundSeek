// https://gist.github.com/anonymous/0f19fdd16100c18244f90f488669640a

#ifndef MULTI_ARRAY_SERIALIZATION_HPP
#define MULTI_ARRAY_SERIALIZATION_HPP
#include <iostream>
#include <fstream>
#include <boost/any.hpp>
#include <boost/multi_array.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace boost
{
namespace serialization
{
template <class T>
struct un_any_type
{
};

template <std::size_t K>
struct un_any_int
{
};
///< empty type for recursive template sheningans

template <class Archive, typename T, std::size_t K>
inline void save_multi_array (Archive &ar, const boost::multi_array<T, K> &t, const unsigned int file_version)
{
    typedef boost::multi_array<T, K> multi_array_;
    typedef typename multi_array_::size_type size_;

    for (int k = 0; k < K; ++k)
    {
        size_ nk = (t.shape ()[k]);
        ar << BOOST_SERIALIZATION_NVP (nk);
    }
    ar << boost::serialization::make_array (t.data (), t.num_elements ());
}

template <class Archive, typename T, std::size_t K>
inline void load_multi_array (Archive &ar, boost::multi_array<T, K> &t, const unsigned int file_version)

{
    typedef boost::multi_array<T, K> multi_array_;
    typedef typename multi_array_::size_type size_;

    boost::array<size_, K> extents;
    for (int k = 0; k < K; ++k) ar >> BOOST_SERIALIZATION_NVP (extents[k]);

    t.resize (extents);
    ar >> make_array (t.data (), t.num_elements ());
}

template <class Archive, typename T, int K>
inline void serialize (Archive &ar, boost::multi_array<T, K> &t, const unsigned int file_version)
{
    split_free (ar, t, file_version);
}

template <class Archive, typename T, std::size_t K>
inline void save_any (Archive &ar, un_any_type<T>, un_any_int<K>, std::vector<boost::any>::const_iterator &it,
                      const unsigned int file_version)
{
    if (K == 0)
    {
        if (it->type () == typeid (T))
        {  // case of saving an object

            const std::type_info &ti = it->type ();
            std::string a = ti.name ();
            ar << BOOST_SERIALIZATION_NVP (a);
            auto var = boost::any_cast<T> (*it);
            ar << (var);
        }  // if

        if (it->type () == typeid (T *))
        {  // case of saving a pointer of an object
            const std::type_info &ti = it->type ();
            std::string a = ti.name ();
            ar << BOOST_SERIALIZATION_NVP (a);
            auto var = boost::any_cast<T *> (*it);
            ar << (*var);
        }  // if
    }      // if for K-dim
    else
    {
        if (it->type () == typeid (boost::multi_array<T, K>))
        {  // case of saving a multi-dim array
            const std::type_info &ti = it->type ();
            std::string a = ti.name ();
            ar << BOOST_SERIALIZATION_NVP (a);
            /*auto*/ boost::multi_array<T, K> var = boost::any_cast<boost::multi_array<T, K> > (*it);
        save_multi_array (ar, var, file_version);
        }  // if

    }  // if for K-dim
}

template <class Archive, typename T, std::size_t K>
inline void load_any (Archive &ar, un_any_type<T>, un_any_int<K>, std::vector<boost::any>::iterator it,
                      const unsigned int file_version)
{
    if (K == 0)
    {  // case we deal with object, pointer of object, etc

        T var;
        ar >> (var);
        *it = var;
    }  // if for K-dim
    else
    {
        if (it->type () == typeid (boost::multi_array<T, K>))
        {  // case of saving a multi-dim array
            typedef boost::multi_array<T, K> array_type;
            array_type var;
        load_multi_array (ar, var, file_version);
        }  // if

    }  // if for K-dim
}
}
}

#endif

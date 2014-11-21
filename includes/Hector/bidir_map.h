#pragma once

namespace stdext
{

template
<
    class _Kty,
    class _Ty,
    class _Tr    = hash_compare <_Kty, less< _Kty > >,
    class _Alloc = allocator    < pair< const _Kty, _Ty > > 
>
class bidir_map : public hash_map<_Kty, _Ty, _Tr, _Alloc >
{
public:
    iterator find_value(const _Ty& _val)
    {
        return find_value(begin(), end(), _val);
    }
    const_iterator find_value(const _Ty& _val) const
    {
        return find_value(begin(), end(), _val);
    }
    iterator find_value(iterator _First,  iterator _Last, const _Ty& _val)
    {
        for (iterator i = _First; i != _Last; i++)
            if (_val == (*i).second) return i;
        return _Last;
    }
    const_iterator find_value(const_iterator _First, const_iterator _Last, const _Ty& _val) const
    {
        for (const_iterator i = _First; i != _Last; i++)
            if (_val == (*i).second) return i;
        return _Last;
    }     
};

};
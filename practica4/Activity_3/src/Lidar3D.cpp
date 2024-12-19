//
// Copyright (c) ZeroC, Inc. All rights reserved.
//
//
// Ice version 3.7.6
//
// <auto-generated>
//
// Generated from file `Lidar3D.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#include <Lidar3D.h>
#include <IceUtil/PushDisableWarnings.h>
#include <Ice/LocalException.h>
#include <Ice/ValueFactory.h>
#include <Ice/OutgoingAsync.h>
#include <Ice/InputStream.h>
#include <Ice/OutputStream.h>
#include <IceUtil/PopDisableWarnings.h>

#if defined(_MSC_VER)
#   pragma warning(disable:4458) // declaration of ... hides class member
#elif defined(__clang__)
#   pragma clang diagnostic ignored "-Wshadow"
#elif defined(__GNUC__)
#   pragma GCC diagnostic ignored "-Wshadow"
#endif

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 307
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 >= 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 6
#       error Ice patch level mismatch!
#   endif
#endif

#ifdef ICE_CPP11_MAPPING // C++11 mapping

namespace
{

const ::std::string iceC_RoboCompLidar3D_Lidar3D_ids[2] =
{
    "::Ice::Object",
    "::RoboCompLidar3D::Lidar3D"
};
const ::std::string iceC_RoboCompLidar3D_Lidar3D_ops[] =
{
    "getLidarData",
    "getLidarDataArrayProyectedInImage",
    "getLidarDataProyectedInImage",
    "getLidarDataWithThreshold2d",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};
const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarData_name = "getLidarData";
const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name = "getLidarDataArrayProyectedInImage";
const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name = "getLidarDataProyectedInImage";
const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name = "getLidarDataWithThreshold2d";

}

bool
RoboCompLidar3D::Lidar3D::ice_isA(::std::string s, const ::Ice::Current&) const
{
    return ::std::binary_search(iceC_RoboCompLidar3D_Lidar3D_ids, iceC_RoboCompLidar3D_Lidar3D_ids + 2, s);
}

::std::vector<::std::string>
RoboCompLidar3D::Lidar3D::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector<::std::string>(&iceC_RoboCompLidar3D_Lidar3D_ids[0], &iceC_RoboCompLidar3D_Lidar3D_ids[2]);
}

::std::string
RoboCompLidar3D::Lidar3D::ice_id(const ::Ice::Current&) const
{
    return ice_staticId();
}

const ::std::string&
RoboCompLidar3D::Lidar3D::ice_staticId()
{
    static const ::std::string typeId = "::RoboCompLidar3D::Lidar3D";
    return typeId;
}

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarData(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    auto istr = inS.startReadParams();
    ::std::string iceP_name;
    float iceP_start;
    float iceP_len;
    int iceP_decimationDegreeFactor;
    istr->readAll(iceP_name, iceP_start, iceP_len, iceP_decimationDegreeFactor);
    inS.endReadParams();
    TData ret = this->getLidarData(::std::move(iceP_name), iceP_start, iceP_len, iceP_decimationDegreeFactor, current);
    auto ostr = inS.startWriteParams();
    ostr->writeAll(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarDataArrayProyectedInImage(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    auto istr = inS.startReadParams();
    ::std::string iceP_name;
    istr->readAll(iceP_name);
    inS.endReadParams();
    TDataImage ret = this->getLidarDataArrayProyectedInImage(::std::move(iceP_name), current);
    auto ostr = inS.startWriteParams();
    ostr->writeAll(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarDataProyectedInImage(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    auto istr = inS.startReadParams();
    ::std::string iceP_name;
    istr->readAll(iceP_name);
    inS.endReadParams();
    TData ret = this->getLidarDataProyectedInImage(::std::move(iceP_name), current);
    auto ostr = inS.startWriteParams();
    ostr->writeAll(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarDataWithThreshold2d(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::OperationMode::Normal, current.mode);
    auto istr = inS.startReadParams();
    ::std::string iceP_name;
    float iceP_distance;
    int iceP_decimationDegreeFactor;
    istr->readAll(iceP_name, iceP_distance, iceP_decimationDegreeFactor);
    inS.endReadParams();
    TData ret = this->getLidarDataWithThreshold2d(::std::move(iceP_name), iceP_distance, iceP_decimationDegreeFactor, current);
    auto ostr = inS.startWriteParams();
    ostr->writeAll(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceDispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair<const ::std::string*, const ::std::string*> r = ::std::equal_range(iceC_RoboCompLidar3D_Lidar3D_ops, iceC_RoboCompLidar3D_Lidar3D_ops + 8, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - iceC_RoboCompLidar3D_Lidar3D_ops)
    {
        case 0:
        {
            return _iceD_getLidarData(in, current);
        }
        case 1:
        {
            return _iceD_getLidarDataArrayProyectedInImage(in, current);
        }
        case 2:
        {
            return _iceD_getLidarDataProyectedInImage(in, current);
        }
        case 3:
        {
            return _iceD_getLidarDataWithThreshold2d(in, current);
        }
        case 4:
        {
            return _iceD_ice_id(in, current);
        }
        case 5:
        {
            return _iceD_ice_ids(in, current);
        }
        case 6:
        {
            return _iceD_ice_isA(in, current);
        }
        case 7:
        {
            return _iceD_ice_ping(in, current);
        }
        default:
        {
            assert(false);
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
        }
    }
}
/// \endcond

/// \cond INTERNAL
void
RoboCompLidar3D::Lidar3DPrx::_iceI_getLidarData(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<::RoboCompLidar3D::TData>>& outAsync, const ::std::string& iceP_name, float iceP_start, float iceP_len, int iceP_decimationDegreeFactor, const ::Ice::Context& context)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarData_name);
    outAsync->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarData_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        [&](::Ice::OutputStream* ostr)
        {
            ostr->writeAll(iceP_name, iceP_start, iceP_len, iceP_decimationDegreeFactor);
        },
        nullptr);
}
/// \endcond

/// \cond INTERNAL
void
RoboCompLidar3D::Lidar3DPrx::_iceI_getLidarDataArrayProyectedInImage(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<::RoboCompLidar3D::TDataImage>>& outAsync, const ::std::string& iceP_name, const ::Ice::Context& context)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name);
    outAsync->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        [&](::Ice::OutputStream* ostr)
        {
            ostr->writeAll(iceP_name);
        },
        nullptr);
}
/// \endcond

/// \cond INTERNAL
void
RoboCompLidar3D::Lidar3DPrx::_iceI_getLidarDataProyectedInImage(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<::RoboCompLidar3D::TData>>& outAsync, const ::std::string& iceP_name, const ::Ice::Context& context)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name);
    outAsync->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        [&](::Ice::OutputStream* ostr)
        {
            ostr->writeAll(iceP_name);
        },
        nullptr);
}
/// \endcond

/// \cond INTERNAL
void
RoboCompLidar3D::Lidar3DPrx::_iceI_getLidarDataWithThreshold2d(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<::RoboCompLidar3D::TData>>& outAsync, const ::std::string& iceP_name, float iceP_distance, int iceP_decimationDegreeFactor, const ::Ice::Context& context)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name);
    outAsync->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name, ::Ice::OperationMode::Normal, ::Ice::FormatType::DefaultFormat, context,
        [&](::Ice::OutputStream* ostr)
        {
            ostr->writeAll(iceP_name, iceP_distance, iceP_decimationDegreeFactor);
        },
        nullptr);
}
/// \endcond

/// \cond INTERNAL
::std::shared_ptr<::Ice::ObjectPrx>
RoboCompLidar3D::Lidar3DPrx::_newInstance() const
{
    return ::IceInternal::createProxy<Lidar3DPrx>();
}
/// \endcond

const ::std::string&
RoboCompLidar3D::Lidar3DPrx::ice_staticId()
{
    return Lidar3D::ice_staticId();
}

namespace Ice
{
}

#else // C++98 mapping

namespace
{

const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarData_name = "getLidarData";

const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name = "getLidarDataArrayProyectedInImage";

const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name = "getLidarDataProyectedInImage";

const ::std::string iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name = "getLidarDataWithThreshold2d";

}

/// \cond INTERNAL
::IceProxy::Ice::Object* ::IceProxy::RoboCompLidar3D::upCast(Lidar3D* p) { return p; }

void
::IceProxy::RoboCompLidar3D::_readProxy(::Ice::InputStream* istr, ::IceInternal::ProxyHandle< Lidar3D>& v)
{
    ::Ice::ObjectPrx proxy;
    istr->read(proxy);
    if(!proxy)
    {
        v = 0;
    }
    else
    {
        v = new Lidar3D;
        v->_copyFrom(proxy);
    }
}
/// \endcond

::Ice::AsyncResultPtr
IceProxy::RoboCompLidar3D::Lidar3D::_iceI_begin_getLidarData(const ::std::string& iceP_name, ::Ice::Float iceP_start, ::Ice::Float iceP_len, ::Ice::Int iceP_decimationDegreeFactor, const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarData_name, sync);
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_RoboCompLidar3D_Lidar3D_getLidarData_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_RoboCompLidar3D_Lidar3D_getLidarData_name, ::Ice::Normal, context);
        ::Ice::OutputStream* ostr = result->startWriteParams(::Ice::DefaultFormat);
        ostr->write(iceP_name);
        ostr->write(iceP_start);
        ostr->write(iceP_len);
        ostr->write(iceP_decimationDegreeFactor);
        result->endWriteParams();
        result->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarData_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

::RoboCompLidar3D::TData
IceProxy::RoboCompLidar3D::Lidar3D::end_getLidarData(const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_RoboCompLidar3D_Lidar3D_getLidarData_name);
    ::RoboCompLidar3D::TData ret;
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(ret);
    result->_endReadParams();
    return ret;
}

::Ice::AsyncResultPtr
IceProxy::RoboCompLidar3D::Lidar3D::_iceI_begin_getLidarDataArrayProyectedInImage(const ::std::string& iceP_name, const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name, sync);
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name, ::Ice::Normal, context);
        ::Ice::OutputStream* ostr = result->startWriteParams(::Ice::DefaultFormat);
        ostr->write(iceP_name);
        result->endWriteParams();
        result->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

::RoboCompLidar3D::TDataImage
IceProxy::RoboCompLidar3D::Lidar3D::end_getLidarDataArrayProyectedInImage(const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_RoboCompLidar3D_Lidar3D_getLidarDataArrayProyectedInImage_name);
    ::RoboCompLidar3D::TDataImage ret;
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(ret);
    result->_endReadParams();
    return ret;
}

::Ice::AsyncResultPtr
IceProxy::RoboCompLidar3D::Lidar3D::_iceI_begin_getLidarDataProyectedInImage(const ::std::string& iceP_name, const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name, sync);
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name, ::Ice::Normal, context);
        ::Ice::OutputStream* ostr = result->startWriteParams(::Ice::DefaultFormat);
        ostr->write(iceP_name);
        result->endWriteParams();
        result->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

::RoboCompLidar3D::TData
IceProxy::RoboCompLidar3D::Lidar3D::end_getLidarDataProyectedInImage(const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_RoboCompLidar3D_Lidar3D_getLidarDataProyectedInImage_name);
    ::RoboCompLidar3D::TData ret;
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(ret);
    result->_endReadParams();
    return ret;
}

::Ice::AsyncResultPtr
IceProxy::RoboCompLidar3D::Lidar3D::_iceI_begin_getLidarDataWithThreshold2d(const ::std::string& iceP_name, ::Ice::Float iceP_distance, ::Ice::Int iceP_decimationDegreeFactor, const ::Ice::Context& context, const ::IceInternal::CallbackBasePtr& del, const ::Ice::LocalObjectPtr& cookie, bool sync)
{
    _checkTwowayOnly(iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name, sync);
    ::IceInternal::OutgoingAsyncPtr result = new ::IceInternal::CallbackOutgoing(this, iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name, del, cookie, sync);
    try
    {
        result->prepare(iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name, ::Ice::Normal, context);
        ::Ice::OutputStream* ostr = result->startWriteParams(::Ice::DefaultFormat);
        ostr->write(iceP_name);
        ostr->write(iceP_distance);
        ostr->write(iceP_decimationDegreeFactor);
        result->endWriteParams();
        result->invoke(iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name);
    }
    catch(const ::Ice::Exception& ex)
    {
        result->abort(ex);
    }
    return result;
}

::RoboCompLidar3D::TData
IceProxy::RoboCompLidar3D::Lidar3D::end_getLidarDataWithThreshold2d(const ::Ice::AsyncResultPtr& result)
{
    ::Ice::AsyncResult::_check(result, this, iceC_RoboCompLidar3D_Lidar3D_getLidarDataWithThreshold2d_name);
    ::RoboCompLidar3D::TData ret;
    if(!result->_waitForResponse())
    {
        try
        {
            result->_throwUserException();
        }
        catch(const ::Ice::UserException& ex)
        {
            throw ::Ice::UnknownUserException(__FILE__, __LINE__, ex.ice_id());
        }
    }
    ::Ice::InputStream* istr = result->_startReadParams();
    istr->read(ret);
    result->_endReadParams();
    return ret;
}

/// \cond INTERNAL
::IceProxy::Ice::Object*
IceProxy::RoboCompLidar3D::Lidar3D::_newInstance() const
{
    return new Lidar3D;
}
/// \endcond

const ::std::string&
IceProxy::RoboCompLidar3D::Lidar3D::ice_staticId()
{
    return ::RoboCompLidar3D::Lidar3D::ice_staticId();
}

RoboCompLidar3D::Lidar3D::~Lidar3D()
{
}

/// \cond INTERNAL
::Ice::Object* RoboCompLidar3D::upCast(Lidar3D* p) { return p; }

/// \endcond

namespace
{
const ::std::string iceC_RoboCompLidar3D_Lidar3D_ids[2] =
{
    "::Ice::Object",
    "::RoboCompLidar3D::Lidar3D"
};

}

bool
RoboCompLidar3D::Lidar3D::ice_isA(const ::std::string& s, const ::Ice::Current&) const
{
    return ::std::binary_search(iceC_RoboCompLidar3D_Lidar3D_ids, iceC_RoboCompLidar3D_Lidar3D_ids + 2, s);
}

::std::vector< ::std::string>
RoboCompLidar3D::Lidar3D::ice_ids(const ::Ice::Current&) const
{
    return ::std::vector< ::std::string>(&iceC_RoboCompLidar3D_Lidar3D_ids[0], &iceC_RoboCompLidar3D_Lidar3D_ids[2]);
}

const ::std::string&
RoboCompLidar3D::Lidar3D::ice_id(const ::Ice::Current&) const
{
    return ice_staticId();
}

const ::std::string&
RoboCompLidar3D::Lidar3D::ice_staticId()
{
#ifdef ICE_HAS_THREAD_SAFE_LOCAL_STATIC
    static const ::std::string typeId = "::RoboCompLidar3D::Lidar3D";
    return typeId;
#else
    return iceC_RoboCompLidar3D_Lidar3D_ids[1];
#endif
}

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarData(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    ::Ice::InputStream* istr = inS.startReadParams();
    ::std::string iceP_name;
    ::Ice::Float iceP_start;
    ::Ice::Float iceP_len;
    ::Ice::Int iceP_decimationDegreeFactor;
    istr->read(iceP_name);
    istr->read(iceP_start);
    istr->read(iceP_len);
    istr->read(iceP_decimationDegreeFactor);
    inS.endReadParams();
    TData ret = this->getLidarData(iceP_name, iceP_start, iceP_len, iceP_decimationDegreeFactor, current);
    ::Ice::OutputStream* ostr = inS.startWriteParams();
    ostr->write(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarDataArrayProyectedInImage(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    ::Ice::InputStream* istr = inS.startReadParams();
    ::std::string iceP_name;
    istr->read(iceP_name);
    inS.endReadParams();
    TDataImage ret = this->getLidarDataArrayProyectedInImage(iceP_name, current);
    ::Ice::OutputStream* ostr = inS.startWriteParams();
    ostr->write(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarDataProyectedInImage(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    ::Ice::InputStream* istr = inS.startReadParams();
    ::std::string iceP_name;
    istr->read(iceP_name);
    inS.endReadParams();
    TData ret = this->getLidarDataProyectedInImage(iceP_name, current);
    ::Ice::OutputStream* ostr = inS.startWriteParams();
    ostr->write(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceD_getLidarDataWithThreshold2d(::IceInternal::Incoming& inS, const ::Ice::Current& current)
{
    _iceCheckMode(::Ice::Normal, current.mode);
    ::Ice::InputStream* istr = inS.startReadParams();
    ::std::string iceP_name;
    ::Ice::Float iceP_distance;
    ::Ice::Int iceP_decimationDegreeFactor;
    istr->read(iceP_name);
    istr->read(iceP_distance);
    istr->read(iceP_decimationDegreeFactor);
    inS.endReadParams();
    TData ret = this->getLidarDataWithThreshold2d(iceP_name, iceP_distance, iceP_decimationDegreeFactor, current);
    ::Ice::OutputStream* ostr = inS.startWriteParams();
    ostr->write(ret);
    inS.endWriteParams();
    return true;
}
/// \endcond

namespace
{
const ::std::string iceC_RoboCompLidar3D_Lidar3D_all[] =
{
    "getLidarData",
    "getLidarDataArrayProyectedInImage",
    "getLidarDataProyectedInImage",
    "getLidarDataWithThreshold2d",
    "ice_id",
    "ice_ids",
    "ice_isA",
    "ice_ping"
};

}

/// \cond INTERNAL
bool
RoboCompLidar3D::Lidar3D::_iceDispatch(::IceInternal::Incoming& in, const ::Ice::Current& current)
{
    ::std::pair<const ::std::string*, const ::std::string*> r = ::std::equal_range(iceC_RoboCompLidar3D_Lidar3D_all, iceC_RoboCompLidar3D_Lidar3D_all + 8, current.operation);
    if(r.first == r.second)
    {
        throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
    }

    switch(r.first - iceC_RoboCompLidar3D_Lidar3D_all)
    {
        case 0:
        {
            return _iceD_getLidarData(in, current);
        }
        case 1:
        {
            return _iceD_getLidarDataArrayProyectedInImage(in, current);
        }
        case 2:
        {
            return _iceD_getLidarDataProyectedInImage(in, current);
        }
        case 3:
        {
            return _iceD_getLidarDataWithThreshold2d(in, current);
        }
        case 4:
        {
            return _iceD_ice_id(in, current);
        }
        case 5:
        {
            return _iceD_ice_ids(in, current);
        }
        case 6:
        {
            return _iceD_ice_isA(in, current);
        }
        case 7:
        {
            return _iceD_ice_ping(in, current);
        }
        default:
        {
            assert(false);
            throw ::Ice::OperationNotExistException(__FILE__, __LINE__, current.id, current.facet, current.operation);
        }
    }
}
/// \endcond

/// \cond STREAM
void
RoboCompLidar3D::Lidar3D::_iceWriteImpl(::Ice::OutputStream* ostr) const
{
    ostr->startSlice(ice_staticId(), -1, true);
    ::Ice::StreamWriter< Lidar3D, ::Ice::OutputStream>::write(ostr, *this);
    ostr->endSlice();
}

void
RoboCompLidar3D::Lidar3D::_iceReadImpl(::Ice::InputStream* istr)
{
    istr->startSlice();
    ::Ice::StreamReader< Lidar3D, ::Ice::InputStream>::read(istr, *this);
    istr->endSlice();
}
/// \endcond

/// \cond INTERNAL
void
RoboCompLidar3D::_icePatchObjectPtr(Lidar3DPtr& handle, const ::Ice::ObjectPtr& v)
{
    handle = Lidar3DPtr::dynamicCast(v);
    if(v && !handle)
    {
        IceInternal::Ex::throwUOE(Lidar3D::ice_staticId(), v);
    }
}
/// \endcond

namespace Ice
{
}

#endif

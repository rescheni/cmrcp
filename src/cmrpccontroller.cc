#include "cmrpccontroller.h"

CmrpcController::CmrpcController()
{
    m_failed = false;
    m_errText = "";
}
void CmrpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}

bool CmrpcController::Failed() const
{
    return m_failed;
}

std::string CmrpcController::ErrorText() const
{    
    return m_errText;
}

void CmrpcController::SetFailed(const std::string & reason)
{
    m_failed = true;
    m_errText = reason;
}

// no realize
void CmrpcController::StartCancel(){}
bool CmrpcController::IsCanceled() const {return false;}
void CmrpcController::NotifyOnCancel(google::protobuf::Closure * callback){}

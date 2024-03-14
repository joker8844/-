#include "logininfoinstance.h"

LoginInfoInstance* LoginInfoInstance::m_instance = new LoginInfoInstance;

LoginInfoInstance::LoginInfoInstance()
{

}

LoginInfoInstance* LoginInfoInstance::getInstance()
{
    return m_instance;
}

void LoginInfoInstance::setUser(QString user) {
    this->m_user = user;
}

void LoginInfoInstance::setToken(QString token) {
    this->m_token = token;
}

void LoginInfoInstance::setIp(QString ip) {
    this->m_ip = ip;
}

void LoginInfoInstance::setPort(QString port) {
    this->m_port = port;
}

QString LoginInfoInstance::user() {
    return m_user;
}

QString LoginInfoInstance::token()
{
    return m_token;
}

QString LoginInfoInstance::ip()
{
    return m_ip;
}

QString LoginInfoInstance::port()
{
    return m_port;
}

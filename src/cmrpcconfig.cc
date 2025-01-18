#include "cmrpcconfig.h"


// 负责加载 解析配置文件 
void CmrpcConfig::LoadConfigFile(const char * config_file)
{
    std::ifstream cnf;
    cnf.open(config_file);

    if(cnf.is_open())
    {
        std::string fline;
        while(getline(cnf,fline))
        {
            // '   '  '#'   xxxx=xxxx
            if(fline[0] == '#' || fline.empty())continue;
            
            int t = 0;
            while(1)
            {
                if(fline[t] != ' ')break;
                t++;
            }
            if(!t)
            {
                fline = fline.substr(t);
            }
            
            if((t=fline.find_last_of(' '))!=-1)
            {
                fline = fline.substr(0,t);
            }

            std::string key,val;
            int p = fline.find('=');
            key = fline.substr(0,p);
            val = fline.substr(p+sizeof '=');
            // m_configMap[key] = val;
            m_configMap.insert({key,val});
        }
    }else {
        // open file error
    }
    cnf.close();

}

// 查询配置文件信息
std::string CmrpcConfig::Load(const std::string &key )
{
    auto it = m_configMap.find(key);
    if(it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}
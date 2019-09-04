#ifndef QUERY_WORKER_H
#define QUERY_WORKER_H

#include <ios>      //std::ios_base::failure
#include <iostream> //std::cout

#include <stdexcept>
#include <string>
#include <vector>

#include "adios2/common/ADIOSMPI.h"
#include "adios2/common/ADIOSTypes.h"
#include <fstream>

#include "Index.h"
#include "Query.h"
#include "Util.h"
#include <pugixml.hpp>

namespace adios2
{
namespace query
{
class Worker
{
public:
    // maybe useful?
    // Worker(const pugi::xml_node queryNode, MPI_Comm comm);
    // will add Worker(jsonNode&, MPI_Comm comm);

    virtual ~Worker();

    adios2::core::Engine *GetSourceReader() { return m_SourceReader; }

    // unsigned long Estimate();
    void GetResultCoverage(const adios2::Box<adios2::Dims> &,
                           std::vector<Box<adios2::Dims>> &);

    /*
    template<class T> void GetBlockIndexCoverage(adios2::Engine& bp4Reader,
                                                adios2::query::Query<T>& query,
                                                std::vector<Box<adios2::Dims>>&)
      {
        if (query.IsMultiVar()){
          std::cout<<" This query type Not supported yet"<<std::endl;
        }

      }
    */

    // bool BuildIdxFile(bool overwrite, size_t recommendedSize)
    //{
    // adios2::IO idxWriteIO =
    // m_adios2.DeclareIO(std::string("BLOCKINDEX-Write-")+m_DataReader->Name());
    // adios2::BlockIndexBuilder builder(m_IdxFileName, m_Comm, overwrite);
    // builder.GenerateIndexFrom(*m_DataIO, idxWriteIO, *m_DataReader,
    // recommendedSize);
    //}

    bool HasMoreSteps() { return false; /*tbd*/ }

protected:
    Worker(const std::string &configFile, adios2::core::Engine *adiosEngine);

    // template<class T> Query<T>& GetQuery() {}

    MPI_Comm m_Comm;
    std::string m_QueryFile; // e.g. xml file
    // adios2::core::ADIOS& m_Adios2;
    // adios2::core::IO* m_IO = nullptr;
    adios2::core::Engine *m_SourceReader = nullptr;
    adios2::query::QueryBase *m_Query = nullptr;

private:
    // adios2::IO* m_DataIO = nullptr;

    // std::string m_IdxFileName;

    // adios2::query::AbstractQueryIndex<T>* m_Idx = nullptr;
}; // worker

class XmlWorker : public Worker
{
public:
    XmlWorker(const std::string &configFile, adios2::core::Engine *adiosEngine)
    : Worker(configFile, adiosEngine)
    {
        ParseMe();
    }

    ~XmlWorker()
    {
        if (m_Query != nullptr)
            delete m_Query;
    }
    void ParseMe();

private:
    void ConstructTree(RangeTree &host, const pugi::xml_node &node);
    void ConstructQuery(QueryVar &q, const pugi::xml_node &node);

    void ParseIONode(const pugi::xml_node &ioNode);
    adios2::query::QueryVar *ParseVarNode(const pugi::xml_node &node,
                                          adios2::core::IO &currentIO,
                                          adios2::core::Engine &reader);

}; // XmlWorker

static Worker *GetWorker(const std::string &configFile,
                         adios2::core::Engine *adiosEngine)
{
    std::ifstream fileStream(configFile);

    if (!fileStream)
        throw std::ios_base::failure("ERROR: file " + configFile +
                                     " not found. ");

    if (adios2::query::IsFileNameXML(configFile))
    {
        // return new XmlWorker<double> (configFile, comm);
        return new XmlWorker(configFile, adiosEngine);
    }
    return nullptr;
}

/*
template<class T>
  void  XmlWorker<T>::Parse()
{
  std::cout<<"Parsing xml"<<m_QueryFile<<std::endl;
}
*/
}; // namespace query
}; // name space adios2

#endif // QUERY_WORKER_H
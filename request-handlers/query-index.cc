#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <regex>
#include<stdio.h>
#include<assert.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <unistd.h>
#include<ctime>

#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>

#include "query-index.h"

using std::string;
using std::vector;

using Poco::JSON::Object;
using Poco::JSON::Array;

const GeoIndex *pIndex;
Array::Ptr targets;
int count;
double radius;
std::vector<Object::Ptr> tempResults;

int query(void* in){
  int *t=(int*)in;
  int id=t[0];
  int left=t[1];
  int right=t[2];
  char timelistname[30];
  sprintf(timelistname,"timelist%d.txt",id);
  FILE *timelist;
  timelist=fopen(timelistname,"w");
  if(timelist!=NULL) fprintf(timelist,"id:%d left:%d right:%d\n",id,left,right);

  for(int i=left;i<right;++i){
    if(timelist!=NULL) fprintf(timelist,"i:%d in:%d ",i,(int)clock());
    Object::Ptr result(new Object);
    Array::Ptr points(new Array);
    Array::Ptr targetPoint=targets->get(i).extract<Array::Ptr>();

    const std::string &id=targetPoint->get(0).toString();
    const double lat=(double)targetPoint->get(1);
    const double lng=(double)targetPoint->get(2);

    auto pPoints=pIndex->QueryClosestPoints(GeoPoint(id,lat,lng),count,radius);

    for(auto &point:*pPoints){
      points->add((const Array::Ptr)point);
    }

    result->set("points",points);

    tempResults[i]=result;
    if(timelist!=NULL) fprintf(timelist,"out:%d\n",(int)clock());
  }
  return 0;
}

void QueryIndexRequestHandler::handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
  FILE *timelist=fopen("timelist.txt","w");
  // Find the index
  Log("URI", request.getURI());

  pIndex = m_registry.GetGeoIndex(m_uuid);

  if (!pIndex) {
    BadRequest(response);
    return;
  }

  m_performanceLogger.start("parse-request-body");
  const Object::Ptr object = ReadRequestBody(request);
  m_performanceLogger.finish("parse-request-body");

  targets = object->get("points").extract<Array::Ptr>();
  count = object->get("count");
  radius = object->get("radius");

  Object::Ptr objRes(new Object);
  Array::Ptr results(new Array);

  m_performanceLogger.start("make-query");

  int len=targets->size();
  const int block=8;
  const int stack_size=1<<22;
  int remain=len%block;
  //std::vector<decltype(clone())> Pid;
  int left=0;
  char (*stack)[stack_size]=new char [block][stack_size];

  int arg[block][3];
  for(int i=0;i<block;++i){
    if(timelist!=NULL) fprintf(timelist,"begin task id:%d time:%d\n",i,(int)clock());
    int size=len/block+(i<remain?1:0);
    int right=left+size;
    arg[i][0]=i;
    arg[i][1]=left;
    arg[i][2]=right;
    clone(query,stack[i]+stack_size-1,CLONE_VM,(void*)arg[i]);
    left=right;
  }
  assert(left==len);

  if(timelist!=NULL) fprintf(timelist,"now time:%d\n",(int)clock());
  waitpid(-1,NULL,0);
  if(timelist!=NULL) fprintf(timelist,"finish:%d\n",(int)clock());
  delete []stack;

  for(int i=0;i<len;++i) results->add(tempResults[i]);
/*
  for (auto &i: *targets) {
    Object::Ptr result(new Object);
    Array::Ptr points(new Array);

    Array::Ptr targetPoint = i.extract<Array::Ptr>();
    result->set("target", targetPoint);

    const string &id = targetPoint->get(0).toString();
    const double lat = (double)targetPoint->get(1);
    const double lng = (double)targetPoint->get(2);

    auto pPoints = pIndex->QueryClosestPoints(GeoPoint(id, lat, lng), count, radius);

    for (auto &point: *pPoints) {
      points->add((const Array::Ptr)point);
    }

    result->set("points", points);

    results->add(result);
  }
*/
  m_performanceLogger.finish("make-query");

  objRes->set("id", m_uuid);
  objRes->set("results", results);

  SendResponse(response, objRes);
  if(timelist!=NULL) fprintf(timelist,"check:%d\n",(int)clock());
}


#ifndef _ResouceFile_H
#define _ResouceFile_H
#include "zlib/zlib.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable:4996)
class  ResouceDataFile
{

public:
	//获取文件返回错误枚举型
	enum GetAllgpxFilepathFromfolderError
	{
		Succeed,
		Out_of_memory,
		None_Find,
	};
#pragma pack(push,1)
	//资源包文件头部
	struct ResDataHeader
	{
		char Header[4];
		int IndexNumber;
		int IndexPoint;
		int IndexSize;
	};
	//资源包解包索引表
	struct ResDataIndexInfo {
		int pOffset;
		int pNone;
		int pOriginSize;
		int pDataSize;
		void *pFileName;
	};
	//资源包打包索引表
	struct ResDataIndexInfo2 {
		int pNone;
		int pOffset;
		int pOriginSize;
		int pDataSize;
	};
	//资源文件信息;
	struct ResouceFileInfo
	{
		void *FileName;
		int pOffset;
		int pOriginSize;
		int pDataSize;
	};
	//地图位置信息;
	struct ResMapInfo {
		UINT Left;
		UINT Right;
		UINT Width;
		UINT Height;
		UINT ImageCount;
		int MouseLeft;
		int MouseRight;
		int MouseTop;
		int MouseBottom;
		char *MapPath;
	};
	struct ResMapInfoHeader {
		char header[4];
		UINT IndexNumber;
		UINT IndexPoint;
		UINT IndexSize;
	};
#pragma pack(pop)
	 ResouceDataFile();
	~ ResouceDataFile();
	//获取资源包数据索引表(资源包文件名,索引表接收缓冲区)成功返回索引表数量,失败返回0;
	ResDataIndexInfo *GetResDataIndex(char *FilePath);
	//打包资源文件表
	BOOL PackageResouce(char *FilePath);
	//获取资源索引表数量;
	UINT GetResIndexNumber();
	//打包地图表;
	BOOL PackageMap(ResMapInfo *ResMap, UINT MapCount);
private:
	ResDataIndexInfo *GetDataIndexAlloc = nullptr;//全局索引表;
	int DataIndexNumber = 0;//全局索引表数量;
	ResouceFileInfo *GvResfile = nullptr;//全局资源文件表;
	UINT FileCount = 0, MaxCount = 300;//全局资源文件表数,最大文件表数量;
	//截取文件路径;
	char *GetResDirectory(char*dirbuf, char*sourbuf);
	//获取运行目录(ANSII编码)
	void GetExePathA(char*dirbuf, int len);
	//枚举资源文件表文件名;
	GetAllgpxFilepathFromfolderError GetAllgpxFilepathFromfolder(const char*  Path);
	//扩大缓冲区
	void* AmplifyIndexArray(UINT MaxCount);
	//读取文件字符流
	BOOL freadc(FILE *hFile, void *vReadBuffer, UINT ReadLen);
	//写入文件字符流
	BOOL fwritec(FILE *hFile, void *vReadBuffer, UINT ReadLen);

};

#endif
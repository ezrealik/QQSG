#ifndef _ResouceFile_H
#define _ResouceFile_H
#include "../QQSG/zlib/zlib.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma warning(disable:4996)
namespace ResouceFile {
	//全局数据结构
	typedef struct _ImportFile
	{
		char pFile[MAX_PATH];
		UINT AnimateMaxCount;
		BOOL IsAnimate = FALSE;
		_ImportFile *AnimateFile;
	}ImportFile, *PImportFile;
	typedef enum _ImageLoadStyle {
		Image = 0xF1,
		Animate = 0xF2,
		Button = 0xF3,
	}ImageLoadStyle;
	typedef struct _AnimateImage {
		float x;
		float y;
		UINT Width;
		UINT Height;
		float Scale;
		LPDIRECT3DTEXTURE9 Texture;
		WCHAR ImageFile[MAX_PATH];
	}AnimateImage, *PAnimateImage;
	typedef struct _ImageTexturInfo
	{
		float x;
		float y;
		UINT Width;
		UINT Height;
		float Scale;
		ImageLoadStyle ImgLoadType;
		BOOL IsHide;
		LPDIRECT3DTEXTURE9 Texture;
		PAnimateImage Animate;
		UINT AnimateTickIndex;
		UINT OldAnimateTick;
		UINT AnimateMaxCout;
		UINT AnimateDelay;
		WCHAR ImageFile[MAX_PATH];
		D3DCOLOR RectAngleColor;
		BOOL HeightLight;
	}ImageTexturInfo, *PImageTexturInfo;
	typedef struct _MapImageInfo
	{
		PImageTexturInfo Image;
		UINT MaxImage;
	}MapImageInfo, *PMapImageInfo;
	typedef struct _ReadResMapInfo {
		UINT ImageOffset;
		UINT ImageDataSize;
		UINT ImageOriginSize;
		float x;
		float y;
		UINT Width;
		UINT Height;
		float Scale;
		UINT AnimateDelay;
		ImageLoadStyle ImgLoadType;
		UINT AnimateCount;
		_ReadResMapInfo *Animate;
	}ReadResMapInfo, *PReadResMapInfo;
	class ResourceAlloc {
	public:
		ResourceAlloc();
		~ResourceAlloc();
		void Release();
		UINT GetLenth();
		UINT GetFileCount();
	private:
		void *pAlloc = nullptr;

	};
	//类别名;
	typedef ResourceAlloc ResourceAllocC, *PResourceAlloc;
	//资源数据操作类
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
			UINT IndexNumber;
			UINT IndexPoint;
			UINT IndexSize;
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
			UINT ImageOffset;
			UINT ImageDataSize;
			UINT ImageOriginSize;
			ResMapInfo *Animate;
			UINT AnimateCount;
			UINT LoadStyle;
		};
		struct ResMapOInfo
		{
			UINT MaxCount;
			PReadResMapInfo Mapinfo;
		};
		struct WriteResMapInfo {
			UINT ImageOffset;
			UINT ImageDataSize;
			UINT ImageOriginSize;
			float x;
			float y;
			UINT Width;
			UINT Height;
			float Scale;
			UINT AnimateDelay;
			ImageLoadStyle ImgLoadType;
			UINT AnimateCount;
		};
		struct ResMapInfoHeader {
			char header[4];
			UINT IndexNumber;
			UINT IndexPoint;
			UINT IndexSize;
		};
#pragma pack(pop)
		ResouceDataFile();
		~ResouceDataFile();
		//获取资源包数据索引表(资源包文件名,索引表接收缓冲区)成功返回索引表数量,失败返回0;
		ResDataIndexInfo *GetResDataIndex(const char *FilePath);
		//打包资源文件表
		BOOL PackageResouce(const char *FilePath);
		//获取资源索引表数量;
		UINT GetResIndexNumber();
		//打包地图表;
		BOOL PackageMap(const MapImageInfo &Resmpinfo);
		//获取打包地图文件表
		ResMapOInfo *GetMapImageInfo(const char *FilePath);
		//获取内存打包地图文件表
		ResMapOInfo *GetMapImageInfoImport(const char *FilePath, PImportFile &pImportFile);
		void Release();
	private:
		ResDataIndexInfo *GetDataIndexAlloc = nullptr;//全局索引表;
		int DataIndexNumber = 0;//全局索引表数量;
		ResouceFileInfo *GvResfile = nullptr;//全局资源文件表;
		UINT FileCount = 0, MaxCount = 300;//全局资源文件表数,最大文件表数量;
		ResMapOInfo G_MapImage = { 0 };
		//获取运行目录(ANSII编码)
		void GetExePathA(char*dirbuf, int len);
		//截取文件路径;
		char *GetResDirectory(const char*dirbuf, char*sourbuf);
		//枚举资源文件表文件名;
		GetAllgpxFilepathFromfolderError GetAllgpxFilepathFromfolder(const char*  Path);
		//扩大缓冲区
		void* AmplifyIndexArray(UINT MaxCount);
		ResourceAlloc pResAlloc;
	};
}
#endif
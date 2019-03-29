#include "ResourceFile.h"

ResouceDataFile::ResouceDataFile()
{
	OutputDebugStringA("构造函数开启!\n");
}

ResouceDataFile::~ResouceDataFile()
{
	if (GetDataIndexAlloc) {
		for (int i = 0; i < DataIndexNumber; i++) {
			if (GetDataIndexAlloc[i].pFileName) {
				LocalFree(GetDataIndexAlloc[i].pFileName);
			}
		}
		LocalFree(GetDataIndexAlloc);
	}
	OutputDebugStringA("析构函数开启!\n");
}
//获取资源包数据索引表(资源包文件名,索引表接收缓冲区)成功返回索引表数量,失败返回0. 不使用一定记得释放资源;
ResouceDataFile::ResDataIndexInfo *ResouceDataFile::GetResDataIndex(char *FilePath) {
	FILE *ResFile = nullptr;
	ResDataIndexInfo *ResInfo = nullptr;
	int pathlen = 0;
	ResFile = fopen(FilePath, "rb");
	if (!ResFile) {
		MessageBox(nullptr, L"文件打开失败,请确认是否有权限!", nullptr, MB_OK);
		DataIndexNumber = 0;
		return nullptr;
	}
	ResDataHeader ResHeader;
	fseek(ResFile, 0, SEEK_SET);
	fread(&ResHeader, sizeof(ResHeader), 1, ResFile);
	fseek(ResFile, ResHeader.IndexPoint, SEEK_SET);
	ResInfo = (ResDataIndexInfo*)LocalAlloc(LMEM_ZEROINIT, sizeof(ResDataIndexInfo)*ResHeader.IndexNumber);
	if (!ResInfo) {
		MessageBox(nullptr, L"内存申请失败!请确认是否内存不足!", nullptr, MB_OK);
		fclose(ResFile);
		DataIndexNumber = 0;
		return nullptr;
	}
	for (int i = 0; i < ResHeader.IndexNumber; i++) {
		fread(&pathlen, 2, 1, ResFile);
		ResInfo[i].pFileName = LocalAlloc(LMEM_ZEROINIT, pathlen + 1);
		if (ResInfo[i].pFileName) {
			fread(ResInfo[i].pFileName, pathlen, 1, ResFile);
		}
		else {
			MessageBox(nullptr, L"内存申请失败!请确认是否内存不足!", nullptr, MB_OK);
			break;
		}
		fread(&ResInfo[i].pNone, sizeof(ResInfo[i].pNone), 1, ResFile);
		fread(&ResInfo[i].pOffset, sizeof(ResInfo[i].pOffset), 1, ResFile);
		fread(&ResInfo[i].pOriginSize, sizeof(ResInfo[i].pOriginSize), 1, ResFile);
		fread(&ResInfo[i].pDataSize, sizeof(ResInfo[i].pDataSize), 1, ResFile);
	}
	fclose(ResFile);
	GetDataIndexAlloc = ResInfo;
	DataIndexNumber = ResHeader.IndexNumber;
	return ResInfo;
}
//获取索引表数量;
UINT ResouceDataFile::GetResIndexNumber() {
	return DataIndexNumber;
}
//获取运行目录(ANSII编码)
void ResouceDataFile::GetExePathA(char*dirbuf, int len) {
	GetModuleFileNameA(NULL, dirbuf, len);
	(strrchr(dirbuf, '\\'))[1] = 0;
}
//打包文件资源包(预打包文件目录);
BOOL ResouceDataFile::PackageResouce(char *FilePath) {
	//申请全局预打包资源文件名数组;
	GvResfile = (ResouceFileInfo*)LocalAlloc(LMEM_ZEROINIT, sizeof(ResouceFileInfo) * MaxCount);
	if(!GvResfile)return FALSE;
	//枚举预打包资源文件目录文件;
	if (GetAllgpxFilepathFromfolder(FilePath) != Succeed)return FALSE;
	char ResDir[MAX_PATH] = { 0 };
	//取运行目录;
	GetExePathA(ResDir, MAX_PATH);
	//创建打开资源包文件;
	strcat(ResDir, "ResData.db");
	//重写资源文件数据;
	FILE *OpenResFile = fopen(ResDir, "wb");
	if (!OpenResFile)return FALSE;
	//写入资源包文件头部信息;
	fseek(OpenResFile, 0, SEEK_SET);
	ResDataHeader ResHeader = { 0 };
	fwrite(&ResHeader, sizeof(ResDataHeader), 1, OpenResFile);
	//写入资源文件数据;
	for (UINT i = 0; i < FileCount; i++) {
		//获取文件名指针;
		char *pPath = (char*)GvResfile[i].FileName;
		OutputDebugStringA(pPath);
		OutputDebugStringA("\r\n");
		//打开预打包资源文件;
		if (!pPath) { MessageBox(0, L"打包失败!", nullptr, NULL); break; }
		FILE *TmpFile = fopen(pPath, "rb");
		if (!TmpFile) { MessageBox(0, L"打包失败!", nullptr, NULL); break; }
		//移动预打包资源文件指针并获取文件大小;
		fseek(TmpFile, 0, SEEK_END);
		UINT FileLen = ftell(TmpFile);
		//申请缓冲区;
		void *TmpData = LocalAlloc(LMEM_ZEROINIT, FileLen);
		if (!TmpData)return FALSE;
		//重置预打包资源文件指针;
		fseek(TmpFile, 0, SEEK_SET);
		//读取并打包资源文件;
		UINT ReadLen = fread(TmpData, sizeof(char), FileLen, TmpFile);
		//申请压缩缓冲区;
		void *zlibData = LocalAlloc(LMEM_ZEROINIT, ReadLen);
		if (!zlibData)return FALSE;
		//压缩资源包数据;
		compress((Bytef*)zlibData, (uLongf*)&ReadLen, (Bytef*)TmpData, FileLen);
		GvResfile[i].pOriginSize = FileLen;
		GvResfile[i].pDataSize = ReadLen;
		//写入已压缩数据;
		GvResfile[i].pOffset = ftell(OpenResFile);
		fwrite(zlibData, sizeof(char), ReadLen, OpenResFile);
		//关闭打开的资源文件;
		LocalFree(TmpData);
		LocalFree(zlibData);
		fclose(TmpFile);
		//刷新流缓冲区;
		fflush(OpenResFile);
	}
	//写入资源文件索引表;
	ResHeader.IndexPoint = ftell(OpenResFile);
	memcpy(ResHeader.Header, "PKS", sizeof(ResHeader.Header));//文件头标识
	ResHeader.IndexNumber = FileCount;
	ResouceDataFile::ResDataIndexInfo2 Resinf = { 0 };
	for (UINT i = 0; i < FileCount; i++) {
		//获取资源路径;
		char *pchar = GetResDirectory(FilePath, (char*)GvResfile[i].FileName);
		if (!pchar) { MessageBox(0, L"获取文件目录失败!", 0, 0); return FALSE; }
		//取路径长度
		WORD slens = (WORD)strlen(pchar) + 1;
		//取索引表长度
		ResHeader.IndexSize += (WORD)((sizeof(ResDataIndexInfo2) + slens) + sizeof(WORD));
		//设置索引结构
		Resinf.pNone = 1;
		Resinf.pOffset = GvResfile[i].pOffset;
		Resinf.pOriginSize = GvResfile[i].pOriginSize;
		Resinf.pDataSize = GvResfile[i].pDataSize;
		//写入索引表文件名长度;
		fwrite(&slens, sizeof(WORD), 1, OpenResFile);
		//写入索引表文件名;
		fwrite(pchar, slens, 1, OpenResFile);
		//写入索引结构;
		fwrite(&Resinf, sizeof(ResDataIndexInfo2), 1, OpenResFile);
	}
	//刷新流缓冲区;
	fflush(OpenResFile);
	//写入文件头索引表;
	fseek(OpenResFile, 0, SEEK_SET);
	fwrite(&ResHeader, sizeof(ResDataHeader), 1, OpenResFile);
	fflush(OpenResFile);
	//关闭打开的资源包文件;
	fclose(OpenResFile);
	for (UINT i = 0; i < FileCount; i++) {
		if (GvResfile[i].FileName) {
			LocalFree(GvResfile[i].FileName);
		}
	}
	LocalFree(GvResfile);
	return TRUE;
}
//截取资源路径;
char *ResouceDataFile::GetResDirectory(char*dirbuf, char*sourbuf) {
	int ilen = strlen(sourbuf);
	char *pdir = sourbuf;
	for (int i = 0; i < ilen; i++) {
		if (sourbuf[i] == dirbuf[i])pdir++;
		else { pdir++; break; }
	}
	return pdir;
}
//读取文件字符流处理;
BOOL ResouceDataFile::freadc(FILE *hFile, void *vReadBuffer, UINT ReadLen) {
	char *pBuf = (char*)vReadBuffer;
	char GetChar;
	for (UINT i = 0; i < ReadLen; i++) {
		GetChar = fgetc(hFile);
		if (GetChar == EOF)return FALSE;
		pBuf[i] = GetChar;
	}
	return TRUE;
}
//写入文件字符流处理;
BOOL ResouceDataFile::fwritec(FILE *hFile, void *vReadBuffer, UINT ReadLen) {
	char *pBuf = (char*)vReadBuffer;
	for (UINT i = 0; i < ReadLen; i++) {
		if (i >= 4095) {
			int ir = 0;
		}
		if (fputc(pBuf[i], hFile) == EOF)return FALSE;
	}
	return 0;
}
//扩大全局资源文件表缓冲区;
void* ResouceDataFile::AmplifyIndexArray(UINT MaxCount) {
	void *Alloc = LocalAlloc(LMEM_ZEROINIT, sizeof(ResouceFileInfo) * MaxCount);
	if (!Alloc)return nullptr;
	void *OldAlloc = GvResfile;
	memcpy(&GvResfile, &Alloc, 4);
	memcpy(GvResfile, OldAlloc, sizeof(ResouceFileInfo) *FileCount);
	LocalFree(OldAlloc);
	return Alloc;
}
//获取目录所有文件名(目录,资源文件名数组缓冲区,数组当前计数,数组最大值);
ResouceDataFile::GetAllgpxFilepathFromfolderError ResouceDataFile::GetAllgpxFilepathFromfolder(const char *Path)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATAA FindFileData;
	strcpy(szFind, Path);
	strcat(szFind, "\\*.*");
	HANDLE hFind = FindFirstFileA(szFind, &FindFileData);
	char *RootItem = nullptr;
	if (INVALID_HANDLE_VALUE == hFind)return None_Find;
	do
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				//发现子目录，递归之
				char szFile[MAX_PATH] = { 0 };
				strcpy(szFile, Path);
				strcat(szFile, "\\");
				strcat(szFile, FindFileData.cFileName);
				if (GetAllgpxFilepathFromfolder(szFile) != Succeed)return Out_of_memory;
			}
		}
		else
		{
			char szFile[MAX_PATH] = { 0 };
			strcpy(szFile, Path);
			strcat(szFile, "\\");
			strcat(szFile, FindFileData.cFileName);
			if (FileCount >= MaxCount) {
				MaxCount += 100;
				AmplifyIndexArray(MaxCount);
			}
			UINT szflen = strlen(szFile) + 1;
			if (!szflen)break;
			void *FileAlloc = LocalAlloc(LMEM_ZEROINIT, szflen);
			if (!FileAlloc)return Out_of_memory;
			memcpy(FileAlloc, szFile, szflen);
			GvResfile[FileCount].FileName = FileAlloc;

			FileCount++;
		}
	} while (FindNextFileA(hFind, &FindFileData));
	FindClose(hFind);
	return Succeed;
}
//打包地图表;
BOOL ResouceDataFile::PackageMap(ResMapInfo *ResMap, UINT MapCount) {
	char FileName[256] = { 0 };
	GetExePathA(FileName, sizeof(FileName));
	strcat(FileName, "\\ResMap.map");
	FILE *pFile = fopen(FileName, "wb");
	if (!pFile) return FALSE;
	ResMapInfoHeader ResMapHeader = { 0 };
	strcpy(ResMapHeader.header, "Map");
	fwrite(&ResMapHeader, sizeof(ResMapHeader), 1, pFile);
	void *Tmpdata = LocalAlloc(LMEM_ZEROINIT, (4 * 1024) * 1024);
	if (!Tmpdata)return FALSE;
	for (UINT i = 0; i < MapCount; i++) {
		
	}
	LocalFree(Tmpdata);
	fclose(pFile);
	return TRUE;
}


#include "ResourceFile.hpp"
using namespace ResouceFile;
ResouceDataFile::ResouceDataFile()
{
	//OutputDebugStringA("构造函数开启!\n");
}

ResouceDataFile::~ResouceDataFile()
{
}
void ResouceDataFile::Release() {
	if (GetDataIndexAlloc) {
		for (int i = 0; i < DataIndexNumber; i++) {
			if (GetDataIndexAlloc[i].pFileName) {
				LocalFree(GetDataIndexAlloc[i].pFileName);
			}
		}
		LocalFree(GetDataIndexAlloc);
	}
	if (G_MapImage.Mapinfo) {
		for (UINT i = 0; i < G_MapImage.MaxCount; i++) {
			if (G_MapImage.Mapinfo[i].Animate) {
				LocalFree(G_MapImage.Mapinfo[i].Animate);
			}
		}
		LocalFree(G_MapImage.Mapinfo);
		G_MapImage = { 0 };
	}
}
//获取资源包数据索引表(资源包文件名,索引表接收缓冲区)成功返回索引表数量,失败返回0. 不使用一定记得释放资源;
ResouceDataFile::ResDataIndexInfo *ResouceDataFile::GetResDataIndex(const char *FilePath) {
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
	for (UINT i = 0; i < ResHeader.IndexNumber; i++) {
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
BOOL ResouceDataFile::PackageResouce(const char *FilePath) {
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
char *ResouceDataFile::GetResDirectory(const char*dirbuf, char*sourbuf) {
	int ilen = strlen(sourbuf);
	char *pdir = sourbuf;
	for (int i = 0; i < ilen; i++) {
		if (sourbuf[i] == dirbuf[i])pdir++;
		else { break; }
	}
	return pdir;
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
BOOL ResouceDataFile::PackageMap(const MapImageInfo &Resmpinfo) {
	char FileName[256] = { 0 };
	GetExePathA(FileName, sizeof(FileName));
	strcat(FileName, "\\ResMap.map");
	FILE *pFile = fopen(FileName, "wb");
	if (!pFile) return FALSE;
	fseek(pFile, 0, SEEK_SET);
	ResMapInfoHeader ResMapHeader = { 0 };
	strcpy(ResMapHeader.header, "Map");
	fwrite(&ResMapHeader, sizeof(ResMapHeader), 1, pFile);
	ResMapInfo *ResMp = (ResMapInfo*)LocalAlloc(LMEM_ZEROINIT, sizeof(ResMapInfo)*Resmpinfo.MaxImage);
	if (!ResMp)return FALSE;
	UINT IndexNum = Resmpinfo.MaxImage;
	for (UINT i = 0; i < Resmpinfo.MaxImage; i++) {
		if (Resmpinfo.Image[i].IsHide) { IndexNum--; continue; }
		if (Resmpinfo.Image[i].ImgLoadType == _Image) {
			//UNICODE字符转ANSII字符
			UINT len = wcslen(Resmpinfo.Image[i].ImageFile);
			if (!WideCharToMultiByte(CP_ACP, NULL, Resmpinfo.Image[i].ImageFile, len * sizeof(WCHAR), FileName, sizeof(FileName), NULL, NULL))return FALSE;
			//打开图片数组文件;
			FILE *pNewFile = fopen(FileName, "rb");
			if (!pNewFile) { MessageBox(0, L"文件打开失败!", NULL, NULL); return FALSE; }
			//存储地图索引表信息;
			fseek(pNewFile, 0, SEEK_END);
			//地图索引表原大小;
			UINT TmpLen = ftell(pNewFile);
			ResMp[i].ImageOriginSize = TmpLen;
			fseek(pNewFile, 0, SEEK_SET);
			//申请内存空间;
			void *Tmpdata = LocalAlloc(LMEM_ZEROINIT, TmpLen);
			if (!Tmpdata) { MessageBox(0, L"申请内存失败!", NULL, NULL); return FALSE; }
			//读取要保存的文件;
			UINT ReadLen = fread(Tmpdata, 1, TmpLen, pNewFile);
			void *lzip = LocalAlloc(LMEM_ZEROINIT, compressBound(ReadLen));
			if (!lzip) { MessageBox(0, L"申请内存失败!", NULL, NULL); return FALSE; }
			//压缩内存;
			compress((Bytef*)lzip, (uLongf*)&ReadLen, (Bytef*)Tmpdata, (uLongf)TmpLen);
			ResMp[i].ImageDataSize = ReadLen;
			//地图索引表偏移;
			ResMp[i].ImageOffset = ftell(pFile);
			ResMp[i].LoadStyle = _Image;
			fwrite(lzip, ReadLen, 1, pFile);
			fflush(pNewFile);
			fflush(pFile);
			LocalFree(Tmpdata);
			LocalFree(lzip);
			fclose(pNewFile);
		}
		else if (Resmpinfo.Image[i].ImgLoadType == Animate) {
			ResMapInfo *pAlloc = (ResMapInfo*)LocalAlloc(LMEM_ZEROINIT, sizeof(ResMapInfo)*Resmpinfo.Image[i].AnimateMaxCout);
			if(!pAlloc) { MessageBox(0, L"申请内存失败!", NULL, NULL); return FALSE; }
			ResMp[i].Animate = pAlloc;
			ResMp[i].AnimateCount = Resmpinfo.Image[i].AnimateMaxCout;
			for (UINT n = 0; n < Resmpinfo.Image[i].AnimateMaxCout; n++) {
				//UNICODE字符转ANSII字符
				AnimateImage pAnimate = Resmpinfo.Image[i].Animate[n];
				UINT len = wcslen(pAnimate.ImageFile);
				if (!WideCharToMultiByte(CP_ACP, NULL, pAnimate.ImageFile, len * sizeof(WCHAR), FileName, sizeof(FileName), NULL, NULL))return FALSE;
				//打开图片数组文件;
				FILE *pNewFile = fopen(FileName, "rb");
				if (!pNewFile) { MessageBox(0, L"文件打开失败!", NULL, NULL); return FALSE; }
				//地图索引表偏移;
				fseek(pNewFile, 0, SEEK_END);
				//地图索引表原大小;
				UINT TmpLen = ftell(pNewFile);
				ResMp[i].Animate[n].ImageOriginSize = TmpLen;
				fseek(pNewFile, 0, SEEK_SET);
				//申请内存空间;
				void *Tmpdata = LocalAlloc(LMEM_ZEROINIT, TmpLen);
				if (!Tmpdata) { MessageBox(0, L"申请内存失败!", NULL, NULL); return FALSE; }
				//读取要保存的文件;
				UINT ReadLen = fread(Tmpdata, 1, TmpLen, pNewFile);
				void *lzip = LocalAlloc(LMEM_ZEROINIT, compressBound(ReadLen));
				if (!lzip) { MessageBox(0, L"申请内存失败!", NULL, NULL); return FALSE; }
				//压缩内存;
				compress((Bytef*)lzip, (uLongf*)&ReadLen, (Bytef*)Tmpdata, (uLongf)TmpLen);
				ResMp[i].Animate[n].ImageDataSize = ReadLen;
				ResMp[i].Animate[n].ImageOffset = ftell(pFile);
				ResMp[i].LoadStyle = Animate;
				fwrite(lzip, ReadLen, 1, pFile);
				fflush(pNewFile);
				fflush(pFile);
				LocalFree(Tmpdata);
				LocalFree(lzip);
				fclose(pNewFile);

			}
		}

	}
	ResMapHeader.IndexPoint = ftell(pFile);
	for (UINT i = 0; i < Resmpinfo.MaxImage; i++) {
		if (Resmpinfo.Image[i].IsHide) continue;
		if (ResMp[i].LoadStyle == _Image) {
			ImageTexturInfo pMapInfo = Resmpinfo.Image[i];
			WriteResMapInfo WriteResMap;
			WriteResMap.AnimateDelay = pMapInfo.AnimateDelay;
			WriteResMap.x = pMapInfo.x;
			WriteResMap.y = pMapInfo.y;
			WriteResMap.Height = pMapInfo.Height;
			WriteResMap.Width = pMapInfo.Width;
			WriteResMap.Scale = pMapInfo.Scale;
			WriteResMap.ImgLoadType = pMapInfo.ImgLoadType;
			WriteResMap.ImageDataSize = ResMp[i].ImageDataSize;
			WriteResMap.ImageOffset = ResMp[i].ImageOffset;
			WriteResMap.ImageOriginSize = ResMp[i].ImageOriginSize;
			WriteResMap.AnimateCount = ResMp[i].AnimateCount;
			fwrite(&WriteResMap, sizeof(WriteResMap), 1, pFile);
			fflush(pFile);
			//UNICODE字符转ANSII字符
			UINT stringlen = wcslen(Resmpinfo.Image[i].ImageFile);
			if (!WideCharToMultiByte(CP_ACP, NULL, Resmpinfo.Image[i].ImageFile, stringlen * sizeof(WCHAR), FileName, sizeof(FileName), NULL, NULL))return FALSE;
			//截取索引目录;
			char szPath[MAX_PATH] = { 0 };
			GetExePathA(szPath, sizeof(szPath));
			char *szFileName = GetResDirectory(szPath, FileName);
			//写出索引目录长度;
			UINT szLen = strlen(szFileName);
			fwrite(&szLen, sizeof(szLen), 1, pFile);
			fflush(pFile);
			//写出索引目录名;
			fwrite(szFileName, sizeof(char), szLen, pFile);
			fflush(pFile);
		}
		else if (ResMp[i].LoadStyle == Animate) {
			ImageTexturInfo _pMapInfo = Resmpinfo.Image[i];
			WriteResMapInfo WriteResMap;
			WriteResMap.AnimateDelay = _pMapInfo.AnimateDelay;
			WriteResMap.x = _pMapInfo.x;
			WriteResMap.y = _pMapInfo.y;
			WriteResMap.Height = _pMapInfo.Height;
			WriteResMap.Width = _pMapInfo.Width;
			WriteResMap.Scale = _pMapInfo.Scale;
			WriteResMap.ImgLoadType = _pMapInfo.ImgLoadType;
			WriteResMap.ImageDataSize = ResMp[i].ImageDataSize;
			WriteResMap.ImageOffset = ResMp[i].ImageOffset;
			WriteResMap.ImageOriginSize = ResMp[i].ImageOriginSize;
			WriteResMap.AnimateCount = ResMp[i].AnimateCount;
			fwrite(&WriteResMap, sizeof(WriteResMap), 1, pFile);
			fflush(pFile);
			char Anim[] = "动画帧指针";
			//写出索引目录长度;
			UINT szLen = strlen(Anim);
			fwrite(&szLen, sizeof(szLen), 1, pFile);
			fflush(pFile);
			//写出索引目录名;
			fwrite(Anim, sizeof(char), szLen, pFile);
			fflush(pFile);
			PAnimateImage pMapInfo = Resmpinfo.Image[i].Animate;
			for (UINT n = 0; n < Resmpinfo.Image[i].AnimateMaxCout; n++) {
				WriteResMap.AnimateDelay = Resmpinfo.Image[i].AnimateDelay;
				WriteResMap.x = pMapInfo[n].x;
				WriteResMap.y = pMapInfo[n].y;
				WriteResMap.Height = pMapInfo[n].Height;
				WriteResMap.Width = pMapInfo[n].Width;
				WriteResMap.Scale = pMapInfo[n].Scale;
				WriteResMap.ImgLoadType = Resmpinfo.Image[i].ImgLoadType;
				WriteResMap.ImageDataSize = ResMp[i].Animate[n].ImageDataSize;
				WriteResMap.ImageOffset = ResMp[i].Animate[n].ImageOffset;
				WriteResMap.ImageOriginSize = ResMp[i].Animate[n].ImageOriginSize;
				WriteResMap.AnimateCount = ResMp[i].AnimateCount;
				fwrite(&WriteResMap, sizeof(WriteResMap), 1, pFile);
				fflush(pFile);
				//UNICODE字符转ANSII字符
				UINT stringlen = wcslen(pMapInfo[n].ImageFile);
				if (!WideCharToMultiByte(CP_ACP, NULL, pMapInfo[n].ImageFile, stringlen * sizeof(WCHAR), FileName, sizeof(FileName), NULL, NULL))return FALSE;
				//截取索引目录;
				char szPath[MAX_PATH] = { 0 };
				GetExePathA(szPath, sizeof(szPath));
				char *szFileName = GetResDirectory(szPath, FileName);
				//写出索引目录长度;
				UINT szLen = strlen(szFileName);
				fwrite(&szLen, sizeof(szLen), 1, pFile);
				fflush(pFile);
				//写出索引目录名;
				fwrite(szFileName, sizeof(char), szLen, pFile);
				fflush(pFile);
			}
		}
	}
	ResMapHeader.IndexNumber = IndexNum;
	fseek(pFile, 0, SEEK_SET);
	fwrite(&ResMapHeader, sizeof(ResMapHeader), 1, pFile);
	LocalFree(ResMp);
	fclose(pFile);
	MessageBox(0, L"成功打包地图文件!", NULL, NULL);
	return TRUE;
}
//获取打包地图文件表
ResouceDataFile::ResMapOInfo *ResouceDataFile::GetMapImageInfo(const char *FilePath) {
	FILE *pFile = fopen(FilePath, "rb");
	if (!pFile)return nullptr;
	ResDataHeader MpHeader;
	fread(&MpHeader, sizeof(MpHeader), 1, pFile);
	if (strcmp(MpHeader.Header, "Map") != 0)return nullptr;
	fseek(pFile, MpHeader.IndexPoint, SEEK_SET);
	PReadResMapInfo pMapInfo = (PReadResMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ReadResMapInfo)*MpHeader.IndexNumber);
	if (!pMapInfo)return nullptr;
	G_MapImage.Mapinfo = pMapInfo;
	G_MapImage.MaxCount = MpHeader.IndexNumber;
	for (UINT i = 0; i < MpHeader.IndexNumber; i++) {
		WriteResMapInfo ReadMapInfo;
		fread(&ReadMapInfo, 1, sizeof(WriteResMapInfo), pFile);
		pMapInfo[i].x = ReadMapInfo.x;
		pMapInfo[i].y = ReadMapInfo.y;
		pMapInfo[i].Height = ReadMapInfo.Height;
		pMapInfo[i].Width = ReadMapInfo.Width;
		pMapInfo[i].Scale = ReadMapInfo.Scale;
		pMapInfo[i].ImageOffset = ReadMapInfo.ImageOffset;
		pMapInfo[i].ImgLoadType = ReadMapInfo.ImgLoadType;
		pMapInfo[i].AnimateDelay = ReadMapInfo.AnimateDelay;
		pMapInfo[i].AnimateCount = ReadMapInfo.AnimateCount;
		pMapInfo[i].ImageOriginSize = ReadMapInfo.ImageOriginSize;
		pMapInfo[i].ImageDataSize = ReadMapInfo.ImageDataSize;
		UINT szlen;
		fread(&szlen, 1, sizeof(szlen), pFile);
		char szFile[MAX_PATH];
		fread(szFile, 1, szlen, pFile);
		if (pMapInfo[i].ImgLoadType == Animate && pMapInfo[i].AnimateCount > 0) {
			PReadResMapInfo pTmpAlloc = (PReadResMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ReadResMapInfo)*pMapInfo[i].AnimateCount);
			if (!pTmpAlloc)return nullptr;
			pMapInfo[i].Animate = pTmpAlloc;
			for (UINT n = 0; n < pMapInfo[i].AnimateCount; n++) {
				WriteResMapInfo ReadMapInfo;
				fread(&ReadMapInfo, 1, sizeof(WriteResMapInfo), pFile);
				pTmpAlloc[n].x = ReadMapInfo.x;
				pTmpAlloc[n].y = ReadMapInfo.y;
				pTmpAlloc[n].Height = ReadMapInfo.Height;
				pTmpAlloc[n].Width = ReadMapInfo.Width;
				pTmpAlloc[n].Scale = ReadMapInfo.Scale;
				pTmpAlloc[n].ImageOffset = ReadMapInfo.ImageOffset;
				pTmpAlloc[n].ImgLoadType = ReadMapInfo.ImgLoadType;
				pTmpAlloc[n].AnimateDelay = ReadMapInfo.AnimateDelay;
				pTmpAlloc[n].AnimateCount = ReadMapInfo.AnimateCount;
				pTmpAlloc[n].ImageOriginSize = ReadMapInfo.ImageOriginSize;
				pTmpAlloc[n].ImageDataSize = ReadMapInfo.ImageDataSize;
				UINT szlen;
				fread(&szlen, 1, sizeof(szlen), pFile);
				char szFile[MAX_PATH];
				fread(szFile, 1, szlen, pFile);
			}
		}
	}
	return &G_MapImage;
}
//获取内存打包地图文件表
ResouceDataFile::ResMapOInfo *ResouceDataFile::GetMapImageInfoImport(const char *FilePath, PImportFile &pImportFile) {
	FILE *pFile = fopen(FilePath, "rb");
	if (!pFile)return nullptr;
	ResDataHeader MpHeader;
	fread(&MpHeader, sizeof(MpHeader), 1, pFile);
	if (strcmp(MpHeader.Header, "Map") != 0)return nullptr;
	fseek(pFile, MpHeader.IndexPoint, SEEK_SET);
	PReadResMapInfo pMapInfo = (PReadResMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ReadResMapInfo)*MpHeader.IndexNumber);
	if (!pMapInfo)return nullptr;
	G_MapImage.Mapinfo = pMapInfo;
	G_MapImage.MaxCount = MpHeader.IndexNumber;
	pImportFile = (PImportFile)LocalAlloc(LMEM_ZEROINIT, sizeof(ImportFile)*G_MapImage.MaxCount);
	if (!pImportFile)return nullptr;
	for (UINT i = 0; i < MpHeader.IndexNumber; i++) {
		WriteResMapInfo ReadMapInfo;
		fread(&ReadMapInfo, 1, sizeof(WriteResMapInfo), pFile);
		pMapInfo[i].x = ReadMapInfo.x;
		pMapInfo[i].y = ReadMapInfo.y;
		pMapInfo[i].Height = ReadMapInfo.Height;
		pMapInfo[i].Width = ReadMapInfo.Width;
		pMapInfo[i].Scale = ReadMapInfo.Scale;
		pMapInfo[i].ImageOffset = ReadMapInfo.ImageOffset;
		pMapInfo[i].ImgLoadType = ReadMapInfo.ImgLoadType;
		pMapInfo[i].AnimateDelay = ReadMapInfo.AnimateDelay;
		pMapInfo[i].AnimateCount = ReadMapInfo.AnimateCount;
		pMapInfo[i].ImageOriginSize = ReadMapInfo.ImageOriginSize;
		pMapInfo[i].ImageDataSize = ReadMapInfo.ImageDataSize;
		UINT szlen;
		fread(&szlen, 1, sizeof(szlen), pFile);
		char szFile[MAX_PATH] = { 0 };
		fread(szFile, 1, szlen, pFile);
		strcpy(pImportFile[i].pFile, szFile);
		if (pMapInfo[i].ImgLoadType == Animate && pMapInfo[i].AnimateCount > 0) {
			PReadResMapInfo pTmpAlloc = (PReadResMapInfo)LocalAlloc(LMEM_ZEROINIT, sizeof(ReadResMapInfo)*pMapInfo[i].AnimateCount);
			if (!pTmpAlloc)return nullptr;
			pMapInfo[i].Animate = pTmpAlloc;
			pImportFile[i].AnimateMaxCount = pMapInfo[i].AnimateCount;
			pImportFile[i].IsAnimate = TRUE;
			pImportFile[i].AnimateFile = (PImportFile)LocalAlloc(LMEM_ZEROINIT, sizeof(ImportFile)*pImportFile[i].AnimateMaxCount);
			for (UINT n = 0; n < pMapInfo[i].AnimateCount; n++) {
				WriteResMapInfo ReadMapInfo;
				fread(&ReadMapInfo, 1, sizeof(WriteResMapInfo), pFile);
				pTmpAlloc[n].x = ReadMapInfo.x;
				pTmpAlloc[n].y = ReadMapInfo.y;
				pTmpAlloc[n].Height = ReadMapInfo.Height;
				pTmpAlloc[n].Width = ReadMapInfo.Width;
				pTmpAlloc[n].Scale = ReadMapInfo.Scale;
				pTmpAlloc[n].ImageOffset = ReadMapInfo.ImageOffset;
				pTmpAlloc[n].ImgLoadType = ReadMapInfo.ImgLoadType;
				pTmpAlloc[n].AnimateDelay = ReadMapInfo.AnimateDelay;
				pTmpAlloc[n].AnimateCount = ReadMapInfo.AnimateCount;
				pTmpAlloc[n].ImageOriginSize = ReadMapInfo.ImageOriginSize;
				pTmpAlloc[n].ImageDataSize = ReadMapInfo.ImageDataSize;
				UINT szlen;
				fread(&szlen, 1, sizeof(szlen), pFile);
				char szFile[MAX_PATH] = { 0 };
				fread(szFile, 1, szlen, pFile);
				strcpy(pImportFile[i].AnimateFile[n].pFile, szFile);
			}
		}
	}
	return &G_MapImage;
}
//资源内存类;
ResourceAlloc::ResourceAlloc(){

}
ResourceAlloc::~ResourceAlloc() {
}
void ResourceAlloc::Release() {

}
UINT ResourceAlloc::GetLenth() {
	return 0;
}
UINT ResourceAlloc::GetFileCount() {
	return 0;
}
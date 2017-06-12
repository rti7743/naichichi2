#include "common.h"
#include "XLPOpen2.h"
#include "MainWindow.h"
#if _MSC_VER
#else
#include <sys/types.h> 
#include <sys/wait.h>
#include <poll.h>
#include <sys/stat.h>
#endif

//windows see http://www.ne.jp/asahi/hishidama/home/tech/c/windows/CreatePipe.html
//linux   see http://qiita.com/edo_m18/items/989d2907d4627e19ff65
XLPOpen2::XLPOpen2()
{
	this->Process = NULL;
	this->ReadPipe  = NULL;
	this->WritePipe = NULL;
#if _MSC_VER
	this->ChildReadPipe = 0;
	this->ChildWritePipe = 0;
#endif
}
XLPOpen2::~XLPOpen2()
{
	Stop();
}
string XLPOpen2::Read(int size = 65535)
{
	vector<char> buf(size);
	int readsize = Read(&buf[0],size);
	if (readsize <= -1)
	{
		return "";
	}
	return string(&buf[0],0,readsize);
}
int XLPOpen2::Write(const string& str)
{
	return Write(str.c_str(),str.size());
}

void XLPOpen2::Open(const string& commandLine)
{
	Stop();

#if _MSC_VER
	//書き込み用のパイプを作る
	//自分のthis->WritePipeに書き込むと、子プロセスの this->ChildReadPipe へ届く
	//my write(this->WritePipe) ---> child read(this->ChildReadPipe)
	{
		//パイプを作る
		HANDLE tempPipe;

		// パイプを作成（両ハンドルとも子プロセスへ継承不可）
		if (!CreatePipe(&tempPipe, &this->WritePipe, NULL, 0)) 
		{
			throw XLException("CreatePipe" + XLException::StringWindows());
		}

		// 読込ハンドルを複製（子プロセスへ継承可能な権限の読込ハンドルを作成）
		if (!DuplicateHandle(
			GetCurrentProcess(), tempPipe,
			GetCurrentProcess(), &this->ChildReadPipe,
			0, TRUE, DUPLICATE_SAME_ACCESS)
		) {
			const string errmsg = XLException::StringWindows();

			CloseHandle(tempPipe);
			throw XLException("CreatePipe" + errmsg);
		}

		// 複製元のハンドルは使わないのでクローズ
		if (!CloseHandle(tempPipe)) {
			throw XLException("CloseHandle(tempPipe)" + XLException::StringWindows());
		}
	}

	//読込用のパイプを作る
	//子プロセスが this->ChildWritePipe へ書き込みと 自分の this->ReadPipe へ届く
	//child write(this->ChildWritePipe) ---> my read(this->ReadPipe)
	{
		//パイプを作る
		HANDLE tempPipe;

		SECURITY_ATTRIBUTES sa = {};
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		if (!CreatePipe(&this->ReadPipe, &tempPipe, &sa, 0)) 
		{
			throw XLException("CreatePipe" + XLException::StringWindows());
		}

		// 読込ハンドルを複製（子プロセスへ継承可能な権限の読込ハンドルを作成）
		if (!DuplicateHandle(
			GetCurrentProcess(), tempPipe,
			GetCurrentProcess(), &this->ChildWritePipe,
			0, TRUE, DUPLICATE_SAME_ACCESS)
		) {
			const string errmsg = XLException::StringWindows();

			CloseHandle(tempPipe);
			throw XLException("CreatePipe" + errmsg);
		}

		// 複製元のハンドルは使わないのでクローズ
		if (!CloseHandle(tempPipe)) {
			throw XLException("CloseHandle(tempPipe)" + XLException::StringWindows());
		}
	}

	//プロセスの起動
	BOOL  bInheritHandles = TRUE;
	DWORD creationFlags   = 0;

	STARTUPINFO si = {};
	si.cb         = sizeof(STARTUPINFO);
	si.dwFlags    = STARTF_USESTDHANDLES;
	si.hStdInput  = this->ChildReadPipe;
	si.hStdOutput = this->ChildWritePipe;
	si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);  //親プロセスの標準エラーを継承

	// 子プロセスを起動
	PROCESS_INFORMATION pi = {};
	if (!CreateProcess(
		NULL,
		(char*) commandLine.c_str() ,
		NULL,	//プロセスのセキュリティー記述子
		NULL,	//スレッドのセキュリティー記述子
		bInheritHandles,
		creationFlags,
		NULL,	//環境変数は引き継ぐ
		NULL,	//カレントディレクトリーは同じ
		&si,
		&pi)
	) {
		throw XLException("CreateProcess" + XLException::StringWindows());
	}
	this->Process = pi.hProcess;
	if (!CloseHandle(pi.hThread)) {
		throw XLException("CloseHandle(hThread)" + XLException::StringWindows());
	}
#else
	const int READ = 0;
	const int WRITE = 1;
	// 子から親への通信用パイプ
	int pipe_child2parent[2];

	// 親から子への通信用パイプ
	int pipe_parent2child[2];

	// プロセスID
	int pid;

	// パイプを生成
	if (pipe(pipe_child2parent) < 0) {
		// パイプ生成失敗
		throw XLException("pipe(pipe_child2parent)" + XLException::StringErrNo());
	}

	// パイプを生成
	if (pipe(pipe_parent2child) < 0) {
		int err = errno;

		// 上で開いたパイプを閉じてから終了
		close(pipe_child2parent[READ]);
		close(pipe_child2parent[WRITE]);

		throw XLException("pipe(pipe_child2parent)" + XLException::StringErrNo());
	}
	NOTIFYLOG("fork");

	// fork
	if ((pid = fork()) < 0) {
		// fork失敗
		int err = errno;

		// 開いたパイプを閉じる
		close(pipe_child2parent[READ]);
		close(pipe_child2parent[WRITE]);

		close(pipe_parent2child[READ]);
		close(pipe_parent2child[WRITE]);

		throw XLException("pipe(pipe_child2parent)" + XLException::StringErrNo());
	}

	// 子プロセスか？
	if (pid == 0) {
		// 子プロセスの場合は、親→子への書き込みはありえないのでcloseする
		close(pipe_parent2child[WRITE]);

		// 子プロセスの場合は、子→親の読み込みはありえないのでcloseする
		close(pipe_child2parent[READ]);

		// 親→子への出力を標準入力として割り当て
		dup2(pipe_parent2child[READ], 0);

		// 子→親への入力を標準出力に割り当て
		dup2(pipe_child2parent[WRITE], 1);

		// 割り当てたファイルディスクリプタは閉じる
		close(pipe_parent2child[READ]);
		close(pipe_child2parent[WRITE]);

		// 子プロセスはここで該当プログラムを起動しリターンしない
		if (execl(commandLine.c_str(), commandLine.c_str(), NULL) < 0) {
			int err = errno;
			close(pipe_parent2child[READ]);
			close(pipe_child2parent[WRITE]);
			return ;
		}
		return ;
	}

	NOTIFYLOG("fork ok");
	// 親プロセス側の処理
	close(pipe_parent2child[READ]);
	close(pipe_child2parent[WRITE]);

	this->ReadPipe = pipe_child2parent[READ];
	this->WritePipe = pipe_parent2child[WRITE];
	this->Process = pid;
#endif
}

int XLPOpen2::Read(char* buffer,int size)
{
	ASSERT(this->Process != NULL );
#if _MSC_VER
	DWORD numberOfBytesRead;
	if (!ReadFile(this->ReadPipe, buffer, size, &numberOfBytesRead, NULL)) {
		return -1;
	}
	return (int)numberOfBytesRead;
#else
	return read(this->ReadPipe, buffer, size);
#endif
}

int XLPOpen2::Write(const char* buffer,int size)
{
	ASSERT(this->Process != NULL );
#if _MSC_VER
	DWORD numberOfBytesWritten;
	if (!WriteFile(this->WritePipe, buffer, size, &numberOfBytesWritten, NULL)) {
		return -1;
	}
	return (int)numberOfBytesWritten;
#else
	return write(this->WritePipe, buffer, size);
#endif
}

//読めるようになるまで待機
void XLPOpen2::WaitForReadble()
{
#if _MSC_VER
	DWORD dwRead = 0;
	for(;;)
	{
		if ( ::PeekNamedPipe(this->ReadPipe, NULL, 0, NULL, &dwRead, NULL) == 0 )
		{//プロセスの強制断
			break;
		}
		if (dwRead!=0)
		{//データが来た
			break;
		}
		//windowsのOVERLAPPEDはわかりにくいので、 CPUを使って待ちます.
		SecSleep(1000);
	}
#else
	fd_set fds, readfds;
	FD_ZERO(&readfds);
	FD_SET(this->ReadPipe, &readfds);

	select(this->ReadPipe+1, &fds, NULL, NULL, NULL);
#endif

}

void XLPOpen2::Stop()
{
#if _MSC_VER
	if (this->ChildReadPipe != NULL) {
		CloseHandle(this->ChildReadPipe);
		this->ChildReadPipe = NULL;
	}
	if (this->ChildWritePipe != NULL) {
		CloseHandle(this->ChildWritePipe);
		this->ChildWritePipe = NULL;
	}
	if (this->ReadPipe != NULL) {
		CloseHandle(this->ReadPipe);
		this->ReadPipe = NULL;
	}
	if (this->WritePipe != NULL) {
		CloseHandle(this->WritePipe);
		this->WritePipe = NULL;
	}
	if (this->Process != NULL) {
		CloseHandle(this->Process);
		this->Process = NULL;
	}
#else
	if (this->ReadPipe != 0) {
		close(this->ReadPipe);
		this->ReadPipe = 0;
	}
	if (this->WritePipe != 0) {
		close(this->WritePipe);
		this->WritePipe = 0;
	}
	if (this->Process != 0) {
		close(this->Process);
		this->Process = 0;
	}
#endif
}

bool XLPOpen2::IsExiteProcess()
{
#if _MSC_VER
	if(this->Process == NULL )
	{
		return true;
	}
	return WaitForSingleObject(this->Process, 0) != WAIT_TIMEOUT;
#else
	if(this->Process == 0 )
	{
		return true;
	}
	const string filename = string() + "/proc/" + num2str(this->Process) + "/cmdline";
	struct stat st;
	return stat(filename.c_str() ,&st)  == 0;
#endif
}

// 子プロセスの終了待ち
void XLPOpen2::Join()
{
#if _MSC_VER
	WaitForSingleObject(this->Process, INFINITE);
#else
	int status;
	waitpid(this->Process, &status, WNOHANG);
#endif
}

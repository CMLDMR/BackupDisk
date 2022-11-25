#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Windows.h"

#include <QDebug>
#include <iostream>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")



std::wstring GetRootDriveOfFilePath(const std::wstring &filePath)
{
// get drive #      http://msdn.microsoft.com/en-us/library/windows/desktop/bb773612(v=vs.85).aspx
int drvNbr = PathGetDriveNumber(filePath.c_str());

if (drvNbr == -1)   // fn returns -1 on error
    return L"";

wchar_t buff[4] = {};   // temp buffer for root

// Turn drive number into root      http://msdn.microsoft.com/en-us/library/bb773567(v=vs.85)
PathBuildRoot(buff,drvNbr);

return std::wstring(buff);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto list = this->getListOfDrives();

    for( const auto &item : list ){
        qDebug() << item.c_str();
    }


    qDebug() <<


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Test()
{

    printf("hello world\n");
    DWORD nRead;
    char buf[512];

    HANDLE hDisk = CreateFile(L"\\\\.\\PhysicalDrive0",
        GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, 0, NULL);

    printf("hDisk %i\n", hDisk);

    SetFilePointer(hDisk, 0xA00, 0, FILE_BEGIN);
    ReadFile(hDisk, buf, 512, &nRead, NULL);
    for (int currentpos=0;currentpos < 512;currentpos++) {
        std::cout << buf[currentpos];
    }
    CloseHandle(hDisk);

}

std::vector<std::string> MainWindow::getListOfDrives()
{

    std::vector<std::string> arrayOfDrives;
        char* szDrives = new char[MAX_PATH]();
        if (GetLogicalDriveStringsA(MAX_PATH, szDrives));
        for (int i = 0; i < 100; i += 4)
            if (szDrives[i] != (char)0)
                arrayOfDrives.push_back(std::string{szDrives[i],szDrives[i+1],szDrives[i+2]});
        delete[] szDrives;
        return arrayOfDrives;

}


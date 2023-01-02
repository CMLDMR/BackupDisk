#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include <QDebug>
#include <iostream>
#include <QStorageInfo>
#include <QSysInfo>

#include <Windows.h>
#include <winioctl.h>


int ReadSector(int numSector,BYTE* buf){

    int retCode = 0;
    BYTE sector[512];
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFile(L"\\\\?\\Volume{ef4bf01f-48cc-423a-b189-1d1159e71d19}\\",    // Drive to open
                        GENERIC_READ,           // Access mode
                        FILE_SHARE_READ,        // Share Mode
                        NULL,                   // Security Descriptor
                        OPEN_EXISTING,          // How to create
                        0,                      // File attributes
                        NULL);                  // Handle to template

    if(device != NULL)
    {
        qDebug() << "Drive EXIST";

        SetFilePointer (device, numSector*512, NULL, FILE_BEGIN) ;

        if (!ReadFile(device, sector, 512, &bytesRead, NULL))
        {
            printf("Error in reading disk\n");
        }
        else
        {
            // Copy boot sector into buffer and set retCode
            memcpy(buf,sector, 512);
            retCode=1;
        }

        CloseHandle(device);
        // Close the handle
    }else{
        qDebug() << "Drive NULL";
    }

    return retCode;

}


BOOL GetDriveGeometry(LPCWSTR wszPath, DISK_GEOMETRY *pdg)
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined
    BOOL bResult   = FALSE;                 // results flag
    DWORD junk     = 0;                     // discard results

    hDevice = CreateFile(wszPath,          // drive to open
                         0,                // no access to the drive
                         FILE_SHARE_READ | // share mode
                         FILE_SHARE_WRITE,
                         NULL,             // default security attributes
                         OPEN_EXISTING,    // disposition
                         0,                // file attributes
                         NULL);            // do not copy file attributes

    if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
    {
        return (FALSE);
    }

    bResult = DeviceIoControl(hDevice,                       // device to be queried
                              IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
                              NULL, 0,                       // no input buffer
                              pdg, sizeof(*pdg),            // output buffer
                              &junk,                         // # bytes returned
                              (LPOVERLAPPED) NULL);          // synchronous I/O

    CloseHandle(hDevice);

    return (bResult);
}

std::vector<std::string> getListOfDrives() {

    std::vector<std::string> arrayOfDrives;

    char* szDrives = new char[MAX_PATH]();

    if (GetLogicalDriveStringsA(MAX_PATH, szDrives)){
        for (int i = 0; i < 100; i += 4){
            if (szDrives[i] != (char)0)
                arrayOfDrives.push_back(std::string{szDrives[i],szDrives[i+1],szDrives[i+2]});
        }
    }
    delete[] szDrives;

    return arrayOfDrives;
}

#define wszDrive L"\\\\?\\Volume{6214ca0b-0000-0000-0000-100000000000}\\"

void MainWindow::extracted() {
    for (const auto &item : QStorageInfo::mountedVolumes()) {
        qDebug() << "device" << item.device();
        qDebug() << "blockSize" << item.blockSize();
        qDebug() << "bytesAvailable" << item.bytesAvailable();
        qDebug() << "bytesFree" << item.bytesFree();
        qDebug() << "bytesTotal" << item.bytesTotal();
        qDebug() << "displayName" << item.displayName();
        qDebug() << "fileSystemType" << item.fileSystemType();
        qDebug() << "name" << item.name();
        qDebug() << "rootPath" << item.rootPath();
        qDebug() << "subvolume" << item.subvolume();
        qDebug() << "isReadOnly" << item.isReadOnly();
        qDebug() << "isReady" << item.isReady();
        qDebug() << "isRoot" << item.isRoot();
        qDebug() << "";

        break;
    }
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //    char * drv="\\\\.\\C:";
    //    char *dsk="\\\\.\\PhysicalDrive0";
    //    int sector=0;

    //    char *buff=new char[512];
    //    ReadSect(dsk,buff,sector);
    //    if((unsigned char)buff[510]==0x55 && (unsigned char)buff[511]==0xaa) cout <<"Disk is bootable!"<<endl;
    BYTE *buf = new BYTE[512];
    qDebug() << ReadSector(0,buf);
    for( int i = 0 ; i < 512 ; i++ ){
        qDebug() << i << buf[i];
    }

    auto listDrive = getListOfDrives();

    for( const auto &item : listDrive ){
        qDebug() << item.c_str();
    }

    extracted();

    DISK_GEOMETRY pdg = { {{0}} }; // disk drive geometry structure
    BOOL bResult = FALSE;      // generic results flag
    ULONGLONG DiskSize = 0;    // size of the drive, in bytes
    LPCWSTR path = L"\\\\.\\PhysicalDrive2";
    bResult = GetDriveGeometry (path, &pdg);

    if (bResult)
    {
        qDebug() << "Drive Path: " << path;
        //        std::cout << "Cylinders: " << pdg.Cylinders;

        wprintf(L"Drive path      = %ws\n",   wszDrive);
        wprintf(L"Cylinders       = %I64d\n", pdg.Cylinders);
        wprintf(L"Tracks/cylinder = %ld\n",   (ULONG) pdg.TracksPerCylinder);
        wprintf(L"Sectors/track   = %ld\n",   (ULONG) pdg.SectorsPerTrack);
        wprintf(L"Bytes/sector    = %ld\n",   (ULONG) pdg.BytesPerSector);

        DiskSize = pdg.Cylinders.QuadPart * (ULONG)pdg.TracksPerCylinder *
                (ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;
        wprintf(L"Disk size       = %I64d (Bytes)\n"
                L"                = %.2f (Gb)\n",
                DiskSize, (double) DiskSize / (1024 * 1024 * 1024));
        qDebug() << DiskSize <<(double) DiskSize / (1024 * 1024);
    }
    else
    {
        qDebug() <<  ("GetDriveGeometry failed. Error %ld.\n") <<  GetLastError ();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}








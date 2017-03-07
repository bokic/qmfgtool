#include "qmfgtoolcommandboot.h"
#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QDir>

#include <qendian.h>


QMfgToolCommandBoot::QMfgToolCommandBoot(const QString &file, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QMfgToolCommand(state, body, description, ifdev, parent)
    , m_file(file)
{
}

QMfgToolCommandBoot::~QMfgToolCommandBoot()
{
}

bool QMfgToolCommandBoot::prepare(const QString &chip)
{
    QString filename;

    filename = QApplication::applicationDirPath() + QDir::separator() + QStringLiteral("Profiles") + QDir::separator() + chip + QDir::separator() + QStringLiteral("OS Firmware") + QDir::separator() + m_file;

    m_fileHandle.setFileName(filename);

    if (m_fileHandle.open(QIODevice::ReadOnly))
    {
        m_data = m_fileHandle.readAll();
        m_fileContent = m_data.data();

        return true;
    }

    return false;
}

bool QMfgToolCommandBoot::exec(QMfgToolDevice &device)
{
    const quint32 IVT_BARKER_HEADER	 = 0x402000D1;
    const quint32 IVT_BARKER2_HEADER = 0x412000D1;
    const quint32 HAB_TAG_DCD = 0xd2;
    const quint32 HAB_DCD_BYTES_MAX	= 1768;

#pragma pack(push, 1)
    typedef struct
    {
        uint32_t IvtBarker;
        uint32_t ImageStartAddr;
        uint32_t Reserved;
        uint32_t DCDAddress;
        uint32_t BootData;
        uint32_t SelfAddr;
        uint32_t Reserved2[2];
    } *PIvtHeader;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct _BootData
    {
        uint32_t ImageStartAddr;
        uint32_t ImageSize;
        uint32_t PluginFlag;
    } *PBootData;
#pragma pack(pop)

    // pMxHidDevice->RunPlugIn(m_pDataBuf, m_qwFileSize))
    quint32 *pPlugIn = nullptr;
    qint32 ImgIVTOffset = 0;
    qint64 dwFileSize = 0;
    quint32 PhyRAMAddr4KRL = 0;
    PIvtHeader pIVT = nullptr;
    PBootData pPluginDataBuf = nullptr;

    //Search for IVT
    pPlugIn = static_cast<quint32 *>(m_fileContent);
    dwFileSize = m_fileHandle.size();

    //ImgIVTOffset indicates the IVT's offset from the beginning of the image.
    while(ImgIVTOffset < dwFileSize &&
        (pPlugIn[ImgIVTOffset / sizeof(qint32)] != IVT_BARKER_HEADER &&
         pPlugIn[ImgIVTOffset / sizeof(qint32)] != IVT_BARKER2_HEADER
        ))
    {
        ImgIVTOffset += 0x100;
    }

    if(ImgIVTOffset >= dwFileSize)
    {
        return false;
    }

    pIVT = reinterpret_cast<PIvtHeader>(pPlugIn + ImgIVTOffset / sizeof(quint32));

    //Now we have to judge DCD way or plugin way used in the image
    //The method is to check plugin flag in boot data region
    // IVT boot data format
    //   0x00    IMAGE START ADDR
    //   0x04    IMAGE SIZE
    //   0x08    PLUGIN FLAG
    pPluginDataBuf = reinterpret_cast<PBootData>(pPlugIn + (ImgIVTOffset / sizeof(quint32)) + ((pIVT->BootData - pIVT->SelfAddr) / sizeof(quint32)));

    if(pPluginDataBuf->PluginFlag)
    {
        // TODO: Implement me!
        return false;
    }
    else
    {
        if(pIVT->DCDAddress)
        {
            //DCD mode
            quint32 *pDCDRegion = pPlugIn + ImgIVTOffset/sizeof(quint32) + (pIVT->DCDAddress - pIVT->SelfAddr)/sizeof(quint32);
            //i.e. DCD_BE  0xD2020840              ;DCD_HEADR Tag=0xd2, len=64*8+4+4, ver= 0x40
            //i.e. DCD_BE  0xCC020404              ;write dcd cmd headr Tag=0xcc, len=64*8+4, param=4
            //The first 2 32bits data in DCD region is used to give some info about DCD data.

            //Here big endian format is used, so it must be converted.
            if((qbswap(*pDCDRegion) >> 24) != HAB_TAG_DCD)
            {
                return false;
            }

            if(device.deviceChipFamily() >= MX6Q)
            {
                //The DCD_WRITE command handling was changed from i.MX508.
                //Now the DCD is  performed by HAB and therefore the format of DCD is the same format as in regular image.
                //The DCD_WRITE parameters require size and address. Size is the size of entire DCD file including header.
                //Address is the temporary address that USB will use for storing the DCD file before processing.
                quint32 DCDHeader = qbswap(*pDCDRegion);

                //Total dcd data bytes:
                quint32 TotalDCDDataCnt = (DCDHeader & 0x00FFFF00) >> 8;

                if(TotalDCDDataCnt > HAB_DCD_BYTES_MAX)
                {
                    return false;
                }

                qDebug() << "Before DCDWrite";

                if (!DCDWrite(device, (char *)pDCDRegion, TotalDCDDataCnt))
                {
                    qDebug() << "After with error DCDWrite";

                    return false;
                }

                qDebug() << "After DCDWrite";
            }
            else
            {
                // TODO: Implement me!
                return false;
            }
        }

        //Download boot data to ram
        PhyRAMAddr4KRL = pIVT->SelfAddr - ImgIVTOffset;
        pIVT->DCDAddress = 0;

        if (!transferData(device, PhyRAMAddr4KRL, dwFileSize, (char *)pPlugIn))
        {
            return false;
        }

        device.setLoadAddress(pIVT->SelfAddr);
    }

    return true;
}

bool QMfgToolCommandBoot::DCDWrite(QMfgToolDevice &device, const char data[], quint32 count)
{
    const quint32 ROM_WRITE_ACK = 0x128A8A12;
    const quint32 MAX_DCD_WRITE_REG_CNT = 85;
    const quint16 ROM_KERNEL_CMD_DCD_WRITE = 0x0A0A;

    typedef struct _RomFormatDCDData
    {
        quint32 format;
        quint32 addr;
        quint32 data;
    }RomFormatDCDData, *PRomFormatDCDData;

    SDPCmd SDPCmd;
    SDPCmd.command = ROM_KERNEL_CMD_DCD_WRITE;
    SDPCmd.format = 0;
    SDPCmd.data = 0;
    SDPCmd.address = 0;

    if(device.deviceChipFamily() == MX50)
    {
        //i.mx50
        while(count)
        {
            //The data count here is based on register unit.
            SDPCmd.dataCount = (count > MAX_DCD_WRITE_REG_CNT) ? MAX_DCD_WRITE_REG_CNT : count;
            count -= SDPCmd.dataCount;
            quint32 ByteCnt = SDPCmd.dataCount * sizeof(RomFormatDCDData);

            if(!sendCmd(device, SDPCmd))
            {
                return false;
            }

            if(!sendData(device, data, ByteCnt))
            {
                return false;
            }

            if (!getCmdAck(device, ROM_WRITE_ACK) )
            {
                return false;
            }

            data += ByteCnt;
        }
    }
    else
    {
        SDPCmd.dataCount = count;
        SDPCmd.address = 0x00910000;//IRAM free space

        qDebug() << "Before sendCmd.";

        if(!sendCmd(device, SDPCmd))
        {
            qDebug() << "After with error sendCmd.";

            return false;
        }

        qDebug() << "After sendCmd.";

        quint32 MaxHidTransSize = 1025 - 1;

        while(count > 0)
        {
            qint32 ByteCntTransfered = (count > MaxHidTransSize) ? MaxHidTransSize : count;
            count -= ByteCntTransfered;

            qDebug() << "Before sendData.";

            if(!sendData(device, data, ByteCntTransfered))
            {
                qDebug() << "After with error sendData.";

                return false;
            }

            qDebug() << "After sendData.";

            qDebug() << "Before getCmdAck.";

            if (!getCmdAck(device, ROM_WRITE_ACK) )
            {
                qDebug() << "After with error getCmdAck.";

                return false;
            }

            qDebug() << "After getCmdAck.";

            data += ByteCntTransfered;
            SDPCmd.address += ByteCntTransfered;
        }
    }

    return true;
}

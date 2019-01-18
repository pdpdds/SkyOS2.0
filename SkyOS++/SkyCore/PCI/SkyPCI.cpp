#include "SkyPCI.h"
#include "windef.h"
#include "memory.h"
#include "Hal.h"
#include "sprintf.h"
#include "SystemAPI.h"
#include "intrinsic.h"

PCI	pci;

typedef struct tag_PCIDeviceName
{
	UCHAR	c[3];
	char	*pName;
} PCIDeviceName;

static PCIDeviceName	pci_name[] = {
	{ { 0   ,   0,    1 }, "SCSI Controller"			},
	{ { 0xFF,   1,    1 }, "IDE Controller"				},
	{ { 0	,   0,    2	}, "Ethernet Controller"		},
	{ { 0	,   4,    2	}, "ISDN Controller"			},
	{ { 0	,   0,    3	}, "VGA Controller"				},
	{ { 0	,   0,    4	}, "Video Device"				},
	{ { 0	,   1,    4	}, "Audio Device"				},
	{ { 0	,   0,    6	}, "Host/PCI Bridge"			},
	{ { 0	,   1,    6	}, "PCI/ISA Bridge"				},
	{ { 0	, 128,    6	}, "Other Bridge"				},
	{ { 0	,   2,    6	}, "PCI/EISA Bridge"			},
	{ { 0	,   4,    6	}, "PCI/PCI Bridge"				},
	{ { 0	,   5,    6	}, "PCI/PCMCIA Bridge"			},
	{ { 0	,   3,    7	}, "Generic Modem"				},
	{ { 0	,   3,   12	}, "USB Controller"				},

	{ { 0,   0,    0    }, NULL }
};

// display the pci device's configuration area.
void DisplayPCIParameteter( PCIDevice *pPci )
{
    printf( "UINT16  wVendorID        : %x\n", pPci->cfg.wVendorID );
    printf( "UINT16  wDeviceID        : %x\n", pPci->cfg.wDeviceID );
    printf( "UINT16  wCmdRegister     : %x\n", pPci->cfg.wCmdRegister );
    printf( "UINT16  wStatusRegister  : %x\n", pPci->cfg.wStatusRegister );
    printf( "UCHAR   byRevisionID     : %x\n", pPci->cfg.byRevisionID );
    printf( "UCHAR   class_code[0]    : %x\n", pPci->cfg.class_code[0] );
    printf( "UCHAR   class_code[1]    : %x\n", pPci->cfg.class_code[1] );
    printf( "UCHAR   class_code[2]    : %x\n", pPci->cfg.class_code[2] );
    printf( "UCHAR   byCacheLineSize  : %x\n", pPci->cfg.byCacheLineSize );
    printf( "UCHAR   byLatencyTimer   : %x\n", pPci->cfg.byLatencyTimer );
    printf( "UCHAR   byHeaderType     : %x\n", pPci->cfg.byHeaderType );
    printf( "UCHAR   byBIST           : %x\n", pPci->cfg.byBIST );
    printf( "DWORD   base_addr[0]     : %x\n", pPci->cfg.base_addr[0] );
    printf( "DWORD   base_addr[1]     : %x\n", pPci->cfg.base_addr[1] );
    printf( "DWORD   base_addr[2]     : %x\n", pPci->cfg.base_addr[2] );
    printf( "DWORD   base_addr[3]     : %x\n", pPci->cfg.base_addr[3] );
    printf( "DWORD   base_addr[4]     : %x\n", pPci->cfg.base_addr[4] );
    printf( "DWORD   base_addr[5]     : %x\n", pPci->cfg.base_addr[5] );
    printf( "DWORD   dwCISPointer     : %x\n", pPci->cfg.dwCISPointer );
    printf( "UINT16  wSubVendorID     : %x\n", pPci->cfg.wSubVendorID );
    printf( "UINT16  wSubSystemID     : %x\n", pPci->cfg.wSubSystemID );
    printf( "DWORD   dwRomBaseAddr    : %x\n", pPci->cfg.dwRomBaseAddr );
	printf( "UCHAR   byCapabilityBase : %x\n", pPci->cfg.byCapabilityBase );
	printf( "UCHAR   rsv0[3]          : %x\n", pPci->cfg.rsv0[3] );
	printf( "UCHAR   rsv1[4]          : %x\n", pPci->cfg.rsv1[4] );
	printf( "UCHAR   byIntLine        : %x\n", pPci->cfg.byIntLine );
	printf( "UCHAR   byIntPin         : %x\n", pPci->cfg.byIntPin );
	printf( "UCHAR   byMinGnt         : %x\n", pPci->cfg.byMinGnt );
    printf( "UCHAR   byMaxLat         : %x\n", pPci->cfg.byMaxLat );
}

// find pci device with class codes
int FindPCIDevice( int nIndex, PCIDevice *pPCI, UCHAR *pClass )
{
	int nI;

	for( nI = nIndex; nI < pci.nTotal; nI++ )
	{
		if( memcmp( pci.ent[nI].cfg.class_code, pClass, 3 ) == 0 )
		{
			memcpy( pPCI, &pci.ent[nI], sizeof( PCIDevice ) );
			return( nI );	// found~
		}
	}

	return( -1 );			// not found!
}

static char *get_pci_name( PCICfg *pCfg )
{
	int nI;

	for( nI = 0; pci_name[nI].pName != NULL; nI++ )
	{	
		if( pCfg->class_code[2] == pci_name[nI].c[2] && pCfg->class_code[1] == pci_name[nI].c[1] )
			return( pci_name[nI].pName );		  
	}

	return( NULL );
}

static int display_pci_info( PCI *pPCI )
{
	int		nI;
	char	*pS, szT[128];

	for( nI = 0; nI < pPCI->nTotal; nI++ )
	{
		pS = get_pci_name( &pPCI->ent[nI].cfg );
		if( pS == NULL )
		{
			sprintf( szT, "Unknown PCI Device ( %d.%d.%d )", pPCI->ent[nI].cfg.class_code[2], 
				pPCI->ent[nI].cfg.class_code[1], pPCI->ent[nI].cfg.class_code[0] );
			pS = szT;
		}

		printf( "[%d] %s", nI, pS );
		if( pPCI->ent[nI].cfg.byIntLine == 0 )
			printf( "\n" );
		else
			printf( " IRQ-%d Base Address : %x\n", pPCI->ent[nI].cfg.byIntLine, pPCI->ent[nI].cfg.base_addr[0]);
	}	
	
	printf( "Total %d pci devices.\n", nI );
	
	return( 0 );
}

static int read_pci_port_dword( int nBus, int nDevFn, int nIndex, DWORD *pDword )
{
	DWORD dwX;

	dwX = (DWORD)( (DWORD)0x80000000 | (DWORD)( nBus << 16 ) | (DWORD)( nDevFn << 8 ) |
		           (DWORD)( nIndex << 2 ) );

	// write pci address register
	OutPortDWord( (DWORD)0xCF8, dwX );

	// read pci data register
	*pDword = (DWORD)InPortDWord( (DWORD)0xCFC);

	return( 0 );
}

static int read_pci_config_data( PCICfg *pCfg, int nBus, int nDevFn )
{
	int		nI;
	DWORD	*pX;

	memset( pCfg, 0, sizeof( PCICfg ) );
	   	
	pX = (DWORD*)pCfg;
	for( nI = 0; nI < 16; nI++ )
	{
		read_pci_port_dword( nBus, nDevFn, nI, &pX[nI] );
		if( nI == 0 )
		{
			if( pX[0] == (DWORD)0xFFFFFFFF || pX[0] == 0 || pX[0] == (DWORD)0xFFFF0000 || pX[0] == (DWORD)0x0000FFFF )
				return( -1 );
		}	
	}

	printf( "bus(%d) devfn(%d), device id (%X), revision %d , class ( %d, %d, %d )\n", nBus, nDevFn, pCfg->wDeviceID, pCfg->byRevisionID, pCfg->class_code[0], pCfg->class_code[1], pCfg->class_code[2] );
																 
	return( 0 );
}	

int ScanPCIDevices()
{
	UCHAR			c[3];
	int				nI, nR, nBus, nDevFn;
	
	nI = 0;
	for( nBus = 0; nBus < 256; nBus++ )
	{
		for( nDevFn = 0; nDevFn < 256; nDevFn++ )
		{
			nR =  read_pci_config_data( &pci.ent[nI].cfg, nBus, nDevFn );
			if( nR == 0 )
			{
				if( pci.ent[nI].cfg.class_code[0] == c[0] && 
					pci.ent[nI].cfg.class_code[1] == c[1] &&
					pci.ent[nI].cfg.class_code[2] == c[2]  )
					continue;		

				c[0] = pci.ent[nI].cfg.class_code[0]; 
				c[1] = pci.ent[nI].cfg.class_code[1]; 
				c[2] = pci.ent[nI].cfg.class_code[2];
				
				nI++;
				if( nI >= MAX_PCI_DEVICE )
					goto RETURN;
			}
		}
	}	

RETURN:
	pci.nTotal = nI;

	display_pci_info( &pci );

	return( nI );
}


#ifndef __PCI_H__
#define __PCI_H__

#include <PCI.h>

#ifdef __cplusplus
//  #include <VectorMap.h>
#endif

#define __x86_64__
#include "pci_controller.h"
#include "kheap.h"
#include "PCIA.h"
#include "stl/map.h"
#if defined(__INTEL__) || defined(__x86_64__)
#include "./pci/arch/x86/pci_arch_info.h"
#endif

#	define	__HAIKU_PRI_PREFIX_32		"l"
#	define	__HAIKU_PRI_PREFIX_64		"ll"
#	define B_PRId8			"d"
#	define B_PRIi8			"i"
#	define B_PRId16			"d"
#	define B_PRIi16			"i"
#	define B_PRId32			__HAIKU_PRI_PREFIX_32 "d"
#	define B_PRIi32			__HAIKU_PRI_PREFIX_32 "i"
#	define B_PRId64			__HAIKU_PRI_PREFIX_64 "d"
#	define B_PRIi64			__HAIKU_PRI_PREFIX_64 "i"
#	define B_PRIu8			"u"
#	define B_PRIo8			"o"
#	define B_PRIx8			"x"
#	define B_PRIX8			"X"
#	define B_PRIu16			"u"
#	define B_PRIo16			"o"
#	define B_PRIx16			"x"
#	define B_PRIX16			"X"
#	define B_PRIu32			__HAIKU_PRI_PREFIX_32 "u"
#	define B_PRIo32			__HAIKU_PRI_PREFIX_32 "o"
#	define B_PRIx32			__HAIKU_PRI_PREFIX_32 "x"
#	define B_PRIX32			__HAIKU_PRI_PREFIX_32 "X"
#	define B_PRIu64			__HAIKU_PRI_PREFIX_64 "u"
#	define B_PRIo64			__HAIKU_PRI_PREFIX_64 "o"
#	define B_PRIx64			__HAIKU_PRI_PREFIX_64 "x"
#	define B_PRIX64			__HAIKU_PRI_PREFIX_64 "X"

#ifdef __cplusplus

struct PCIDev;

struct PCIBus {
	PCIBus *			next;
	PCIDev *			parent;
	PCIDev *			child;
	uint8				domain;
	uint8				bus;
};

struct PCIDev {
	PCIDev *			next;
	PCIBus *			parent;
	PCIBus *			child;
	uint8				domain;
	uint8				bus;
	uint8				device;
	uint8				function;
	pci_info			info;
#if defined(__INTEL__) || defined(__x86_64__)
	pci_arch_info		arch_info;
#endif
};


struct domain_data {
	// These two are set in PCI::AddController:
	pci_controller *	controller;
	void *				controller_cookie;

	// All the rest is set in PCI::InitDomainData
	int					max_bus_devices;
};


class PCI {
public:
							PCI();
							~PCI();

			void			InitDomainData();
			void			InitBus();

			status_t		AddController(pci_controller *controller,
								void *controller_cookie);

			status_t		GetNthInfo(long index, pci_info *outInfo);

			status_t		ReadConfig(uint8 domain, uint8 bus, uint8 device,
								uint8 function, uint16 offset, uint8 size,
								uint32 *value);
			uint32			ReadConfig(uint8 domain, uint8 bus, uint8 device,
								uint8 function, uint16 offset, uint8 size);
			uint32			ReadConfig(PCIDev *device, uint16 offset,
								uint8 size);

			status_t		WriteConfig(uint8 domain, uint8 bus, uint8 device,
								uint8 function, uint16 offset, uint8 size,
								uint32 value);
			status_t		WriteConfig(PCIDev *device, uint16 offset,
								uint8 size, uint32 value);

			status_t		FindCapability(uint8 domain, uint8 bus,
								uint8 device, uint8 function, uint8 capID,
								uint8 *offset = NULL);
			status_t		FindCapability(PCIDev *device, uint8 capID,
								uint8 *offset = NULL);
			status_t		FindExtendedCapability(uint8 domain, uint8 bus,
								uint8 device, uint8 function, uint16 capID,
								uint16 *offset = NULL);
			status_t		FindExtendedCapability(PCIDev *device,
								uint16 capID, uint16 *offset = NULL);
			status_t		FindHTCapability(uint8 domain, uint8 bus,
								uint8 device, uint8 function, uint16 capID,
								uint8 *offset);
			status_t		FindHTCapability(PCIDev *device,
								uint16 capID, uint8 *offset = NULL);
			
			status_t		ResolveVirtualBus(uint8 virtualBus, uint8 *domain,
								uint8 *bus);

			PCIDev *		FindDevice(uint8 domain, uint8 bus, uint8 device,
								uint8 function);

			void			ClearDeviceStatus(PCIBus *bus, bool dumpStatus);

			void			RefreshDeviceInfo();

			status_t		UpdateInterruptLine(uint8 domain, uint8 bus,
								uint8 device, uint8 function,
								uint8 newInterruptLineValue);

private:
			void			_EnumerateBus(uint8 domain, uint8 bus,
								uint8 *subordinateBus = NULL);

			void			_FixupDevices(uint8 domain, uint8 bus);

			void			_DiscoverBus(PCIBus *bus);
			void			_DiscoverDevice(PCIBus *bus, uint8 dev,
								uint8 function);

			PCIDev *		_CreateDevice(PCIBus *parent, uint8 dev,
								uint8 function);
			PCIBus *		_CreateBus(PCIDev *parent, uint8 domain,
								uint8 bus);

			status_t		_GetNthInfo(PCIBus *bus, long *currentIndex,
								long wantIndex, pci_info *outInfo);
			void			_ReadBasicInfo(PCIDev *dev);
			void			_ReadHeaderInfo(PCIDev *dev);

			void			_ConfigureBridges(PCIBus *bus);
			void			_RefreshDeviceInfo(PCIBus *bus);

			uint32			_BarSize(uint32 bits, uint32 mask);
			size_t			_GetBarInfo(PCIDev *dev, uint8 offset,
								uint32 *address, uint32 *size = 0,
								uint8 *flags = 0, uint32 *highAddress = 0);
			void			_GetRomBarInfo(PCIDev *dev, uint8 offset,
								uint32 *address, uint32 *size = 0,
								uint8 *flags = 0);

			domain_data *	_GetDomainData(uint8 domain);

			status_t		_CreateVirtualBus(uint8 domain, uint8 bus,
								uint8 *virtualBus);

			int				_NumFunctions(uint8 domain, uint8 bus,
								uint8 device);
			PCIDev *		_FindDevice(PCIBus *current, uint8 domain,
								uint8 bus, uint8 device, uint8 function);

private:
	PCIBus *				fRootBus;

	enum { MAX_PCI_DOMAINS = 8 };

	domain_data				fDomainData[MAX_PCI_DOMAINS];
	uint8					fDomainCount;
	bool					fBusEnumeration;

	typedef map<uint8, uint16> VirtualBusMap;

	VirtualBusMap			fVirtualBusMap;
	int						fNextVirtualBus;
};

extern PCI *gPCI;

#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif

status_t	pci_init(void);
void		pci_uninit(void);

long		pci_get_nth_pci_info(long index, pci_info *outInfo);

uint32		pci_read_config(uint8 virtualBus, uint8 device, uint8 function,
				uint16 offset, uint8 size);
void		pci_write_config(uint8 virtualBus, uint8 device, uint8 function,
				uint16 offset, uint8 size, uint32 value);

void		__pci_resolve_virtual_bus(uint8 virtualBus, uint8 *domain, uint8 *bus);

#ifdef __cplusplus
}
#endif

#endif	/* __PCI_H__ */

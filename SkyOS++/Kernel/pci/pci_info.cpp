#include "windef.h"
#define __HAIKU_PCI_BUS_MANAGER_TESTING 1
#include <haiku_PCI.h>
#include <string.h>
#include "pci_info.h"
#include "pci_private.h"
#include "SkyAPI.h"
#include "Errors.h"
#define __x86_64__

#define PCI_VERBOSE	1
#if defined(__INTEL__) || defined(__x86_64__)
// enabling it makes the pci bus_manager binary about 1MB
// some other platforms have issues with floppy image size...
// TODO: Move this define to BuildSetup?
//#define USE_PCI_HEADER 1
#endif

#if USE_PCI_HEADER
#	include "pcihdr.h"
#	include "pci-utils.h"
#endif

const char *get_capability_name(uint8 cap_id);
const char *get_extended_capability_name(uint16 cap_id);


static void
print_pci2pci_bridge_info(const pci_info *info, bool verbose)
{
	TRACE("PCI:   subsystem_id %04x, subsystem_vendor_id %04x\n",
		info->u.h1.subsystem_id, info->u.h1.subsystem_vendor_id);
	TRACE("PCI:   primary_bus %02x, secondary_bus %02x, subordinate_bus %02x,"
		" secondary_latency %02x\n", info->u.h1.primary_bus,
		info->u.h1.secondary_bus, info->u.h1.subordinate_bus, info->u.h1.secondary_latency);
	uint32 io_base = ((uint32)info->u.h1.io_base & 0xf0) << 8;
	if (info->u.h1.io_base & 1)
		 io_base += ((uint32)info->u.h1.io_base_upper16 << 16);
	uint32 io_limit = (((uint32)info->u.h1.io_limit & 0xf0) << 8) + 0xfff;
	if (info->u.h1.io_limit & 1)
		 io_limit += info->u.h1.io_limit_upper16 << 16;
	TRACE("PCI:   I/O window %04" B_PRIx32 "-%04" B_PRIx32 "\n", io_base,
		io_limit);
	uint32 memory_base = ((uint32)info->u.h1.memory_base & 0xfff0) << 16;
	uint32 memory_limit = (((uint32)info->u.h1.memory_limit & 0xfff0) << 16)
		+ 0xfffff;
	TRACE("PCI:   memory window %08" B_PRIx32 "-%08" B_PRIx32 "\n",
		memory_base, memory_limit);
	uint64 prefetchable_memory_base =
		((uint32)info->u.h1.prefetchable_memory_base & 0xfff0) << 16;
	if (info->u.h1.prefetchable_memory_base & 1) {
		prefetchable_memory_base +=
			(uint64)info->u.h1.prefetchable_memory_base_upper32 << 32;
	}
	uint64 prefetchable_memory_limit =
		(((uint32)info->u.h1.prefetchable_memory_limit & 0xfff0) << 16)
		+ 0xfffff;
	if (info->u.h1.prefetchable_memory_limit & 1) {
		prefetchable_memory_limit +=
			(uint64)info->u.h1.prefetchable_memory_limit_upper32 << 32;
	}
	TRACE("PCI:   prefetchable memory window %016" B_PRIx64 "-%016" B_PRIx64 "\n",
		prefetchable_memory_base, prefetchable_memory_limit);
	TRACE("PCI:   bridge_control %04x, secondary_status %04x\n",
			info->u.h1.bridge_control, info->u.h1.secondary_status);
	TRACE("PCI:   interrupt_line %02x, interrupt_pin %02x\n",
			info->u.h1.interrupt_line, info->u.h1.interrupt_pin);
	TRACE("PCI:   ROM base host %08" B_PRIx32 ", pci %08" B_PRIx32 ", size ??\n",
			info->u.h1.rom_base, info->u.h1.rom_base_pci);
	for (int i = 0; i < 2; i++)
		TRACE("PCI:   base reg %d: host %08" B_PRIx32 ", pci %08" B_PRIx32 ", "
			"size %08" B_PRIx32 ", flags %02x\n", i, info->u.h1.base_registers[i],
			info->u.h1.base_registers_pci[i], info->u.h1.base_register_sizes[i],
			info->u.h1.base_register_flags[i]);
}


static void
print_pci2cardbus_bridge_info(const pci_info *info, bool verbose)
{
	TRACE("PCI:   subsystem_id %04x, subsystem_vendor_id %04x\n",
		info->u.h2.subsystem_id, info->u.h2.subsystem_vendor_id);
	TRACE("PCI:   primary_bus %02x, secondary_bus %02x, subordinate_bus %02x, "
		"secondary_latency %02x\n", info->u.h2.primary_bus,
		info->u.h2.secondary_bus, info->u.h2.subordinate_bus,
		info->u.h2.secondary_latency);
	TRACE("PCI:   bridge_control %04x, secondary_status %04x\n",
		info->u.h2.bridge_control, info->u.h2.secondary_status);
	TRACE("PCI:   memory_base_upper32  %08" B_PRIx32 ", memory_base  %08"
		B_PRIx32 "\n", info->u.h2.memory_base_upper32, info->u.h2.memory_base);
	TRACE("PCI:   memory_limit_upper32 %08" B_PRIx32 ", memory_limit %08"
		B_PRIx32 "\n", info->u.h2.memory_limit_upper32, info->u.h2.memory_limit);
	TRACE("PCI:   io_base_upper32  %08" B_PRIx32 ", io_base  %08" B_PRIx32 "\n",
		info->u.h2.io_base_upper32, info->u.h2.io_base);
	TRACE("PCI:   io_limit_upper32 %08" B_PRIx32 ", io_limit %08" B_PRIx32 "\n",
		info->u.h2.io_limit_upper32, info->u.h2.io_limit);
}


static void
print_generic_info(const pci_info *info, bool verbose)
{
	TRACE("PCI:   ROM base host %08" B_PRIx32 ", pci %08" B_PRIx32 ", size "
		"%08" B_PRIx32 "\n", info->u.h0.rom_base, info->u.h0.rom_base_pci,
		info->u.h0.rom_size);
	TRACE("PCI:   cardbus_CIS %08" B_PRIx32 ", subsystem_id %04x, "
		"subsystem_vendor_id %04x\n", info->u.h0.cardbus_cis,
		info->u.h0.subsystem_id, info->u.h0.subsystem_vendor_id);
	TRACE("PCI:   interrupt_line %02x, interrupt_pin %02x, min_grant %02x, "
		"max_latency %02x\n", info->u.h0.interrupt_line, info->u.h0.interrupt_pin,
		info->u.h0.min_grant, info->u.h0.max_latency);
	for (int i = 0; i < 6; i++)
		TRACE("PCI:   base reg %d: host %08" B_PRIx32 ", pci %08" B_PRIx32 ", "
			"size %08" B_PRIx32 ", flags %02x\n", i, info->u.h0.base_registers[i],
			info->u.h0.base_registers_pci[i], info->u.h0.base_register_sizes[i],
			info->u.h0.base_register_flags[i]);
}


static void
print_capabilities(const pci_info *info)
{
	uint16	status;
	uint8	cap_ptr;
	uint8	cap_id;
	int		i;

	TRACE("PCI:   Capabilities: ");

	status = pci_read_config(info->bus, info->device, info->function, PCI_status, 2);
	if (!(status & PCI_status_capabilities)) {
		TRACE(("(not supported)\n"));
		return;
	}

	switch (info->header_type & PCI_header_type_mask) {
		case PCI_header_type_generic:
		case PCI_header_type_PCI_to_PCI_bridge:
			cap_ptr = pci_read_config(info->bus, info->device, info->function, PCI_capabilities_ptr, 1);
			break;
		case PCI_header_type_cardbus:
			cap_ptr = pci_read_config(info->bus, info->device, info->function, PCI_capabilities_ptr_2, 1);
			break;
		default:
			TRACE("(unknown header type)\n");
			return;
	}

	cap_ptr &= ~3;
	if (!cap_ptr) {
		TRACE("empty list)\n");
		return;
	}

	for (i = 0; i < 48; i++) {
		const char *name;
		cap_id  = pci_read_config(info->bus, info->device, info->function, cap_ptr, 1);
		cap_ptr = pci_read_config(info->bus, info->device, info->function, cap_ptr + 1, 1);
		cap_ptr &= ~3;
		if (i) {
			TRACE((", "));
		}
		name = get_capability_name(cap_id);
		if (name) {
			TRACE("%s", name);
		} else {
			TRACE("0x%02x", cap_id);
		}
		if (!cap_ptr)
			break;
	}
	TRACE("\n");
}


static void
print_extended_capabilities(const pci_info *info)
{
	if (pci_find_capability(info->bus, info->device, info->function,
			PCI_cap_id_pcie, NULL) != B_OK)
		return;

	uint16 capPointer = PCI_extended_capability;
	uint32 capability = pci_read_config(info->bus, info->device,
		info->function, capPointer, 4);
	TRACE("PCI:   Extended capabilities: ");
	if (capability == 0 || capability == 0xffffffff) {
		TRACE(("(empty list)\n"));
		return;
	}

	for (int i = 0; i < 48; i++) {
		if (i) {
			TRACE((", "));
		}
		const char *name = get_extended_capability_name(
			PCI_extcap_id(capability));
		if (name) {
			TRACE("%s", name);
		} else {
			TRACE("0x%04" B_PRIx32, PCI_extcap_id(capability));
		}

		capPointer = PCI_extcap_next_ptr(capability) & ~3;
		if (capPointer < PCI_extended_capability)
			break;
		capability = pci_read_config(info->bus, info->device, info->function,
			capPointer, 4);
	}

	TRACE("\n");
}


static void
print_info_basic(const pci_info *info, bool verbose)
{
	uint8 domain;
	uint8 bus;

	__pci_resolve_virtual_bus(info->bus, &domain, &bus);

	TRACE("PCI: [dom %d, bus %2d] bus %3d, device %2d, function %2d: vendor %04x, device %04x, revision %02x\n",
			domain, bus, info->bus /* virtual bus*/,
			info->device, info->function, info->vendor_id, info->device_id, info->revision);
	TRACE("PCI:   class_base %02x, class_function %02x, class_api %02x\n",
			info->class_base, info->class_sub, info->class_api);

	if (verbose) {
#if USE_PCI_HEADER
		const char *venShort;
		const char *venFull;
		get_vendor_info(info->vendor_id, &venShort, &venFull);
		if (!venShort && !venFull) {
			TRACE(("PCI:   vendor %04x: Unknown\n", info->vendor_id));
		} else if (venShort && venFull) {
			TRACE(("PCI:   vendor %04x: %s - %s\n", info->vendor_id, venShort, venFull));
		} else {
			TRACE(("PCI:   vendor %04x: %s\n", info->vendor_id, venShort ? venShort : venFull));
		}
		const char *devShort;
		const char *devFull;
		get_device_info(info->vendor_id, info->device_id, info->u.h0.subsystem_vendor_id, info->u.h0.subsystem_id,
			&devShort, &devFull);
		if (!devShort && !devFull) {
			TRACE(("PCI:   device %04x: Unknown\n", info->device_id));
		} else if (devShort && devFull) {
			TRACE(("PCI:   device %04x: %s (%s)\n", info->device_id, devShort, devFull));
		} else {
			TRACE(("PCI:   device %04x: %s\n", info->device_id, devShort ? devShort : devFull));
		}
		char classInfo[64];
		get_class_info(info->class_base, info->class_sub, info->class_api, classInfo, sizeof(classInfo));
		TRACE(("PCI:   info: %s\n", classInfo));
#endif
	}
	TRACE("PCI:   line_size %x, latency %x, header_type %x, BIST %x\n",
			info->line_size, info->latency, info->header_type, info->bist);

	switch (info->header_type & PCI_header_type_mask) {
		case PCI_header_type_generic:
			print_generic_info(info, verbose);
			break;
		case PCI_header_type_PCI_to_PCI_bridge:
			print_pci2pci_bridge_info(info, verbose);
			break;
		case PCI_header_type_cardbus:
			print_pci2cardbus_bridge_info(info, verbose);
			break;
		default:
			TRACE(("PCI:   unknown header type\n"));
	}

	print_capabilities(info);
	print_extended_capabilities(info);
}


void
pci_print_info()
{
	pci_info info;
	for (long index = 0; B_OK == pci_get_nth_pci_info(index, &info); index++) {
		print_info_basic(&info, PCI_VERBOSE);
	}
}


const char *
get_capability_name(uint8 cap_id)
{
	switch (cap_id) {
		case PCI_cap_id_reserved:
			return "reserved";
		case PCI_cap_id_pm:
			return "PM";
		case PCI_cap_id_agp:
			return "AGP";
		case PCI_cap_id_vpd:
			return "VPD";
		case PCI_cap_id_slotid:
			return "SlotID";
		case PCI_cap_id_msi:
			return "MSI";
		case PCI_cap_id_chswp:
			return "CompactPCIHotSwap";
		case PCI_cap_id_pcix:
			return "PCI-X";
		case PCI_cap_id_ht:
			return "HyperTransport";
		case PCI_cap_id_vendspec:
			return "vendspec";
		case PCI_cap_id_debugport:
			return "DebugPort";
		case PCI_cap_id_cpci_rsrcctl:
			return "cpci_rsrcctl";
		case PCI_cap_id_hotplug:
			return "HotPlug";
		case PCI_cap_id_subvendor:
			return "subvendor";
		case PCI_cap_id_agp8x:
			return "AGP8x";
		case PCI_cap_id_secure_dev:
			return "Secure Device";
		case PCI_cap_id_pcie:
			return "PCIe";
		case PCI_cap_id_msix:
			return "MSI-X";
		case PCI_cap_id_sata:
			return "SATA";
		case PCI_cap_id_pciaf:
			return "AdvancedFeatures";
		default:
			return NULL;
	}
}


const char *
get_extended_capability_name(uint16 cap_id)
{
	switch (cap_id) {
		case PCI_extcap_id_aer:
			return "Advanced Error Reporting";
		case PCI_extcap_id_vc:
			return "Virtual Channel";
		case PCI_extcap_id_serial:
			return "Serial Number";
		case PCI_extcap_id_power_budget:
			return "Power Budgeting";
		case PCI_extcap_id_rcl_decl:
			return "Root Complex Link Declaration";
		case PCI_extcap_id_rcil_ctl:
			return "Root Complex Internal Link Control";
		case PCI_extcap_id_rcec_assoc:
			return "Root Complex Event Collector Association";
		case PCI_extcap_id_mfvc:
			return "MultiFunction Virtual Channel";
		case PCI_extcap_id_vc2:
			return "Virtual Channel 2";
		case PCI_extcap_id_rcrb_header:
			return "RCRB Header";
		case PCI_extcap_id_vendor:
			return "Vendor Unique";
		case PCI_extcap_id_acs:
			return "Access Control Services";
		case PCI_extcap_id_ari:
			return "Alternative Routing Id Interpretation";
		case PCI_extcap_id_ats:
			return "Address Translation Services";
		case PCI_extcap_id_srio_virtual:
			return "Single Root I/O Virtualization";
		case PCI_extcap_id_mrio_virtual:
			return "Multiple Root I/O Virtual";
		case PCI_extcap_id_multicast:
			return "Multicast";
		case PCI_extcap_id_page_request:
			return "Page Request";
		case PCI_extcap_id_amd:
			return "AMD Reserved";
		case PCI_extcap_id_resizable_bar:
			return "Resizable Bar";
		case PCI_extcap_id_dyn_power_alloc:
			return "Dynamic Power Allocation";
		case PCI_extcap_id_tph_requester:
			return "TPH Requester";
		case PCI_extcap_id_latency_tolerance:
			return "Latency Tolerance Reporting";
		case PCI_extcap_id_2ndpcie:
			return "Secondary PCIe";
		case PCI_extcap_id_pmux:
			return "Protocol Multiplexing";
		case PCI_extcap_id_pasid:
			return "Process Address Space Id";
		case PCI_extcap_id_ln_requester:
			return "LN Requester";
		case PCI_extcap_id_dpc:
			return "Downstream Porto Containment";
		case PCI_extcap_id_l1pm:
			return "L1 Power Management Substates";
		default:
			return NULL;
	}
}


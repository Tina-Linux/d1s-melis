#include <string.h>
#include "page.h"
#include "pgtable.h"
#include <log.h>
#include <mmu.h>
#include <barrier.h>

extern void flush_icache_all(void);
extern unsigned long awos_arch_virt_to_phys(unsigned long virtaddr);

pgd_t system_pg_dir[PTRS_PER_PGD] __aligned(PAGE_SIZE);
pmd_t Lv2PageTables[4][PTRS_PER_PMD] __aligned(PAGE_SIZE);

static pgd_t encode_pgd(unsigned long pa, pgprot_t prot)
{
    return pfn_pgd(PFN_DOWN(pa), prot);
}

static pmd_t encode_pmd(unsigned long pa, pgprot_t prot)
{
    return pfn_pmd(PFN_DOWN(pa), prot);
}

static void mmu_create_map_region(pgd_t *pgdir, unsigned long va, unsigned long pa, unsigned long size, pgprot_t prot)
{
    unsigned long pgd_index = pgd_index(va);

    if (size != PGDIR_SIZE)
    {
        __err("direct map area must be 1G size.");
        return;
    }

    if (pgd_val(pgdir[pgd_index]) == 0)
    {
        pgdir[pgd_index] = pfn_pgd(PFN_DOWN(pa), prot);
    }
    else
    {
        __err("already maped on pgdir index %ld.", pgd_index);
        return;
    }

    return;
}

static void clear_pgd(void)
{
    int i = 0;
    int j = 0;

    pgd_t nullpgd = {0};
    pmd_t nullpmd = {0};
    pte_t nullpte = {0};

    for (i = 0; i < PTRS_PER_PGD; i ++)
    {
        system_pg_dir[i] = nullpgd;
    }

    for(i = 0; i < 4; i++)
    {
        for (j = 0; j < PTRS_PER_PMD; j++)
        {
            Lv2PageTables[i][j] = nullpmd;
        }
    }
}

static void mmu_setpgd(unsigned long vaddr_start, unsigned long vaddr_end, unsigned long paddr_start, pgprot_t attr)
{
    pgd_t pgdir;
    int i, nsec;
    pgd_t nullpgd = {0};

    unsigned long pgd_lvl = pgd_index(vaddr_start);
    unsigned long start_sec = (vaddr_start - (pgd_lvl * PGDIR_SIZE)) >> PMD_SHIFT;

    if (pgd_lvl < PTRS_PER_PGD)
    {
        pgdir = system_pg_dir[pgd_lvl];

        if (!memcmp(&nullpgd, &pgdir, sizeof(pgd_t))) {
            system_pg_dir[pgd_lvl] = encode_pgd((unsigned long)&Lv2PageTables[pgd_lvl], PAGE_TABLE);
        }
        nsec = (vaddr_end >> PMD_SHIFT) - (vaddr_start >> PMD_SHIFT);
        for (i = 0; i <= nsec; i++)
        {
            Lv2PageTables[pgd_lvl][i + start_sec] = encode_pmd(paddr_start + (i << PMD_SHIFT), attr);
        }
    }
}

static mem_desc_t sun20iw1p1_memory_layout[] =
{
    {   0x01C00000, 0x01DFFFFF, 0x01C00000, PAGE_KERNEL_SO },
    {   0x02000000, 0x021FFFFF, 0x02000000, PAGE_KERNEL_SO },
    {   0x02400000, 0x025FFFFF, 0x02400000, PAGE_KERNEL_SO },
    {   0x03000000, 0x031FFFFF, 0x03000000, PAGE_KERNEL_SO },
    {   0x03200000, 0x033FFFFF, 0x03200000, PAGE_KERNEL_SO },
    {   0x04000000, 0x041FFFFF, 0x04000000, PAGE_KERNEL_SO },
    {   0x04200000, 0x043FFFFF, 0x04200000, PAGE_KERNEL_SO },
    {   0x04400000, 0x045FFFFF, 0x04400000, PAGE_KERNEL_SO },
    {   0x05000000, 0x053FFFFF, 0x05000000, PAGE_KERNEL_SO },
    {   0x05400000, 0x055FFFFF, 0x05400000, PAGE_KERNEL_SO },
    {   0x05600000, 0x057FFFFF, 0x05600000, PAGE_KERNEL_SO },
    {   0x05800000, 0x05BFFFFF, 0x05800000, PAGE_KERNEL_SO },
    {   0x05C00000, 0x05DFFFFF, 0x05C00000, PAGE_KERNEL_SO },
    {   0x06000000, 0x061FFFFF, 0x06000000, PAGE_KERNEL_SO },
    {   0x07000000, 0x071FFFFF, 0x07000000, PAGE_KERNEL_SO },
    {   0x08000000, 0x081FFFFF, 0x08000000, PAGE_KERNEL_SO },
    {   0x09000000, 0x091FFFFF, 0x09000000, PAGE_KERNEL_SO },
    {   0x10000000, 0x141FFFFF, 0x10000000, PAGE_KERNEL_SO },
    {
        CONFIG_DRAM_PHYBASE,
        CONFIG_DRAM_PHYBASE + CONFIG_DRAM_SIZE - 1,
        CONFIG_DRAM_PHYBASE, PAGE_KERNEL_EXEC
    },
};

void mmu_build_page_dirs(void)
{
    int i;
    for (i = 0; i < sizeof(sun20iw1p1_memory_layout) / sizeof(sun20iw1p1_memory_layout[0]); i ++)
    {
        mmu_setpgd(sun20iw1p1_memory_layout[i].vaddr_start, \
                   sun20iw1p1_memory_layout[i].vaddr_end, \
                   sun20iw1p1_memory_layout[i].paddr_start, \
                   sun20iw1p1_memory_layout[i].attr);
    }
    flush_icache_all();
    barrier();
    wmb();
    rmb();
    mb();
}

void mmu_init(void)
{
    clear_pgd();

#if 1
    /* mmu mapping by level2 tables  */
    mmu_build_page_dirs();

#else
    /* mmu mapping by level1 tables  */
    mmu_create_map_region(system_pg_dir, 0UL, 0UL, \
                          PGDIR_SIZE, \
                          pgprot_noncached(PAGE_KERNEL));

    /* crerate[1G 2G] map, common memory area, weak order, buffered, cached. */
    mmu_create_map_region(system_pg_dir, CONFIG_DRAM_VIRTBASE, \
                          CONFIG_DRAM_PHYBASE, \
                          CONFIG_DRAM_SIZE < PGDIR_SIZE ? PGDIR_SIZE : physical_memory_size, \
                          PAGE_KERNEL_EXEC);
#endif
    return;
}

unsigned long awos_arch_vaddr_map2_phys(unsigned long vaddr)
{
    unsigned long phyadr;

    if (vaddr == 0)
    {
        return 0;
    }

    /* linear map zone, just cal with the offset. */
    if ((vaddr >= CONFIG_DRAM_VIRTBASE) && (vaddr < CONFIG_DRAM_VIRTBASE + CONFIG_DRAM_SIZE))
    {
        return awos_arch_virt_to_phys(vaddr);
    }

    /*modules runing address is virtual address which mapping memory through mmu*/
    phyadr = 0;
    pgd_t *pgd = NULL;
    p4d_t *p4d = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;

	pgd = pgd_offset_k(vaddr);
	if(pgd_none(*pgd) || !pgd_present(*pgd))
	{
		__inf("the [%p]'s pgd table item [%p] is none or invalid!\r\n", vaddr, pgd_val(*pgd));
		goto __out;
	}
	else
	{
		/* the real 1G item is in pud level, mmuv39 is only 3 levels, the 5 level is code structure */
		if(pgd_val(*pgd) & (_PAGE_READ | _PAGE_EXEC))
		{
			phyadr  = __pa((unsigned long)pgd_page_vaddr(*pgd) + (vaddr & (PGDIR_SIZE - 1)));
			__inf("the [%p]'s pgd level physic address is 0x%p!\r\n", vaddr, phyadr);
			goto __out;
		}
	}

	p4d = p4d_offset(pgd, vaddr);
	if(p4d_none(*p4d) || !p4d_present(*p4d))
	{
		__inf("the [%p]'s p4d table item [%p] is none or invalid!\r\n", vaddr, p4d_val(*p4d));
		goto __out;
	}

	pud = pud_offset(p4d, vaddr);
	if(pud_none(*pud) || !pud_present(*pud))
	{
		__inf("the [%p]'s pud table item [%p] is none or invalid!\r\n", vaddr, pud_val(*pud));
		goto __out;
	}

	pmd = pmd_offset(pud, vaddr);
	if(pmd_none(*pmd) || !pmd_present(*pmd))
	{
		__inf("the [%p]'s pmd table item [%p] is none or invalid!\r\n", vaddr, pmd_val(*pmd));
		goto __out;
	}
	else
	{
		if(pmd_val(*pmd) & (_PAGE_READ | _PAGE_EXEC))
		{
			phyadr  = __pa((unsigned long)pmd_page_vaddr(*pmd) + (vaddr & (PMD_SIZE - 1)));
			__inf("the [%p]'s pmd level physic address is %p!\r\n", vaddr, phyadr);
			goto __out;
		}
	}

	pte = pte_offset_kernel(pmd, vaddr);
	if(pte_none(*pte) || !pte_present(*pte))
	{
		__inf("the [%p]'s pte table item [%p] is none or invalid!\r\n", vaddr, pte_val(*pte));
		goto __out;
	}
	else
	{
		if(pte_val(*pte) & (_PAGE_READ | _PAGE_EXEC))
		{
			phyadr  = __pa((unsigned long)pte_page(*pte) + (vaddr & (PAGE_SIZE - 1)));
			__inf("the [%p]'s pte level physic address is %p!\r\n", vaddr, phyadr);
			goto __out;
		}
	}
	__err("if this log present, maybe need goto checking the vaddr request procedure!\r\n");

__out:
    return phyadr;
}

int32_t check_virtual_address(unsigned long vaddr)
{
    if(0 == awos_arch_vaddr_map2_phys(vaddr))
    {
        /*  invalid */
        return 0;
    }
    /* valid  */
    return 1;
}

/*   (C) Renesas Technology Europe Limited 2006, All rights reserved    */
/*===========================================================================*/
/**
 * @file init_drv.c
 *
 * Initializes the drivers and I/O interface.
 *
 * %full_filespec:init_drv.c~8:csrc:kok_aud#5 %
 * @version %version:8 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed Aug 20 09:19:50 2008 %
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "errno.h"
#include "init_drv.h"
#include "ioif_cfg.h"
#include "init_drv_cfg.h"

#include <assert.h>
#include "sh_errno.h"
#include "drv_scif.h"
#include "scif_if.h"

#ifdef DMA__CFG
#   include "dma_if.h"
#endif

#ifdef IOIF_SIMULATED_IO_DEVICE_PATH
#   include "sim_io_if.h"
#endif

#ifdef MOUNT__CFG
#   include "automount_if.h"
#   include "iso9660fs_if.h"
#   include "vfatfs_if.h"
#endif

#ifdef ROM_DEC__CFG
#  include "romdec_if.h"
#endif

#ifdef SDHI__CFG
#   include "sdhi_if.h"
#endif /* SDHI__CFG */

#ifdef SOUND__CFG
#   include "sound_if.h"
#endif

#ifdef SRC__CFG
#   include "src_if.h"
#endif

#ifdef SSI__CFG
#   include "drv_ssi_if.h"
#endif

#ifdef SSU__CFG
#   include "ssu_if.h"
#endif

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

#ifdef TEST_AUTOMOUNT_NOTIFY
void test_block_notify(const char *mount_name, const char *block_device_name, bool_t mounting)
{
   printf("test_block_notify: %s %s %d\r\n", mount_name, block_device_name, mounting);
}
#endif

/*===========================================================================*\
 *
 * Please refer to the detailed description in init_drv.h
 *
\*===========================================================================*/
void init_drv(void)
{
   ER ercd;
   IOIF_CONFIG ioif_cfg;

   /*
    * Initialise the IOIF mount table.
    */
   ioif_cfg.max_devices = IOIF_MAX_DEVICES;
   ioif_cfg.max_drivers = IOIF_MAX_DRIVERS;
   ioif_cfg.max_open_files = IOIF_MAX_FILES;
   ioif_cfg.aio_task_priority = IOIF_AIO_PRIORITY;

   ercd = ioif_initialise(&ioif_cfg);
   assert(!iserrno(ercd));

   /*
    * Initialize the serial driver.
    */
#ifdef IOIF_SCIF_DEVICE_PATH
   {
      T_SERIAL_SCIF_CONFIG scif_cfg[SCIF_UNIT_MAX];
      int i;

      for (i = 0; i < SCIF_UNIT_MAX; i++)
      {
         scif_cfg[i].int_level = IOIF_SCIF_INT_LVL;
         scif_cfg[i].setup.bit_rate = IOIF_SCIF_BAUD_RATE;
         scif_cfg[i].setup.data_bits = IOIF_SCIF_DATA_BITS;
         scif_cfg[i].setup.stop_bits = IOIF_SCIF_STOP_BITS;
         scif_cfg[i].setup.parity = IOIF_SCIF_PARITY;
      }

      ercd = ioif_start_device(scif_api(), &scif_cfg, IOIF_SCIF_DEVICE_PATH, "", NULL);
      assert(!iserrno(ercd));
   }
#endif /* IOIF_SCIF_DEVICE_PATH */

   /*
    * Initialize the simulated I/O driver.
    */
#ifdef IOIF_SIMULATED_IO_DEVICE_PATH
   ercd = ioif_start_device(sim_io_api(), NULL, IOIF_SIMULATED_IO_DEVICE_PATH, "", NULL);
   assert(!iserrno(ercd));
#endif /* IOIF_SIMULATED_IO_DEVICE_PATH */

   /*
    * Initialise the DMA driver.
    */
#ifdef DMA__CFG
   {
      DMA_INIT_T init;
      int i;

      /* Initialise DMA library */
      init.cms_mode = DMA_CMS_NORMAL;
      init.priority_mode = DMA_PR_FIX0;
      for (i = 0; i < DMA_CH_MAX; i++)
      {
         init.intlvl[i] = DMA_INT_LVL;
      }
      ercd = dma_init(&init);
      assert(!iserrno(ercd));
   }
#endif

   /*
    * Initialise the SSU driver.
    */
#ifdef SSU__CFG
   ercd = ioif_start_device(ssu_api(), NULL, "dev\\ssu", "", NULL);
   assert(!iserrno(ercd));
#endif

   /*
    * Initialise the SRC driver.
    */
#ifdef SRC__CFG
   {
      SRC_DRV_INIT_DATA_T src_cfg;

      src_cfg.input_dmac_chan = -1;
      src_cfg.output_dmac_chan = -1;
      ercd = ioif_start_device(src_api(), &src_cfg, "dev\\src", "", NULL);
      assert(!iserrno(ercd));
   }
#endif

   /*
    * Initialise the SOUND driver.
    */

#ifdef SOUND__CFG
#   ifndef SSI__CFG
#      define SSI__CFG
#   endif
#endif

#ifdef SSI__CFG
   {
      T_SERIAL_SSI_CONFIG ssi_cfg[4];

      ssi_cfg[0].int_level = SSI_INT_LVL;
      ssi_cfg[1].int_level = SSI_INT_LVL;
      ssi_cfg[2].int_level = SSI_INT_LVL;
      ssi_cfg[3].int_level = SSI_INT_LVL;

      /* Initialise the SSI driver */
      ercd = ioif_start_device(ssi_api(), &ssi_cfg, "dev\\ssi", "", NULL);
      assert(!iserrno(ercd));
   }
#endif

#ifdef SOUND__CFG
   {
      T_SOUND_CONFIG sound_cfg;
      sound_cfg.int_level = 5;

      /* Initialise the SOUND driver */
      ercd = ioif_start_device(sound_api(), &sound_cfg, "dev\\sound", "", NULL);
      assert(!iserrno(ercd));
   }
#endif

   /* Initialise the Fake CD driver */
#ifdef FAKECD__CFG
   /* block device number, 2 */
   ercd = ioif_start_device(fakecd_api(), NULL, FAKECD_MOUNTNAME_1, "", NULL);
   assert(!iserrno(ercd));
   /* block device number, 3 */
   ercd = ioif_start_device(fakecd_api(), NULL, FAKECD_MOUNTNAME_2, "", NULL);
   assert(!iserrno(ercd));
   /* block device number, 4 */
   ercd = ioif_start_device(fakecd_api(), NULL, FAKECD_MOUNTNAME_3, "", NULL);
   assert(!iserrno(ercd));
#endif

   /* Initialise the ROM DEC driver */
#ifdef ROM_DEC__CFG
   {
      ROM_DEC_CONFIG_T config;

      config.sync_mode = ROM_DEC_SYNC_MODE_SY_AUTO;
      config.decode_mode.descrambling = 0;
      config.decode_mode.auto_mode = 0;
      config.edc_ecc_check.checking = ROM_DEC_EDC_ECC_CHECK_MODE_PQ;
      config.edc_ecc_check.iterations = ROM_DEC_EDC_ECC_ITERATIONS_ONE;
      config.auto_buf.auto_buffering = 0;
      config.auto_buf.buffer_to_ram = 0;
      config.auto_buf.start_mode = MODE_0;
      config.auto_buf_time.minutes = 0;
      config.auto_buf_time.seconds = 0;
      config.auto_buf_time.frames = 0;
      config.ssi_data.byteswap = 0;
      config.ssi_data.bitswap = 0;
      config.in_dmac_chan = -1;
      config.out_dmac_chan = -1;
      config.ssi_chan = 0;
      config.intr_isec_callback = NULL;
      config.intr_itarg_callback = NULL;
      config.intr_isy_callback = NULL;
      config.intr_ierr_callback = NULL;
      config.intr_ibuf_callback = NULL;
      config.intr_iready_callback = NULL;

      ercd = ioif_start_device(romdec_api(), &config, "dev\\romdec", "", NULL);
      assert(!iserrno(ercd));
   }
#endif

   /* Initialise the SDHI driver */
#ifdef SDHI__CFG
   {
      SDHI_DRV_CONFIG_T sdhi_cfg;

      sdhi_cfg.int_level = SDHI_INT_LVL;
      sdhi_cfg.transfer_mode = SD_MODE_DMA;
      sdhi_cfg.speed_mode = SD_MODE_HS;
      sdhi_cfg.capacity_mode = SD_MODE_VER2X;
      sdhi_cfg.media_type = SD_MODE_MEM;
      sdhi_cfg.media_voltage = SD_VOLT_3_3;
      sdhi_cfg.swap_detection = SD_CD_SOCKET;

      ercd = ioif_start_device(sdhi_api(), &sdhi_cfg, SDHI_MOUNTNAME, "", NULL);
      assert(!iserrno(ercd));
   }
#endif


   /* Initialise the automounter */
#ifdef MOUNT__CFG
   {
#   ifndef AUTOMOUNT__CFG
      /* Use a fixed mount configuration */
      static T_VFATFS_CONFIG vfatfs_cfg = {
         0
      };
      static T_ISO9660FS_CONFIG isofs_cfg = {
         ISO9660_DEFAULT_BUFFER_SIZE,
         2                      /* support 2 CD devices */
      };
      int vfat_fs_handle, iso_fs_handle;
      BLOCK_FILESYSTEM_MOUNT_T fsm;

      /* register file systems */
      vfat_fs_handle = ioif_register_driver(vfatfs_api(), &vfatfs_cfg);
      iso_fs_handle = ioif_register_driver(iso9660fs_api(), &isofs_cfg);

      /* register fixed media in SDHI drive as VFAT mounted on \sd1 */
      fsm.block_device_name = SDHI_MOUNTNAME;
      fsm.flags = O_RDWR;
      fsm.block_device_handle = open(fsm.block_device_name, (int)fsm.flags, 0);
      assert(!iserrno(fsm.block_device_handle));
      fsm.block_device_number = 0;
      fsm.mount_name = VFATFS_MOUNTNAME_4;

      ercd = ioif_mount_device(fsm.mount_name, "A:", vfat_fs_handle, (void *)&fsm);
      assert(!iserrno(ercd));

      /* register fixed media in CD drive as ISO mounted on \cdrom1 */
      fsm.block_device_name = FAKECD_MOUNTNAME_1;
      fsm.flags = O_RDONLY;
      fsm.block_device_handle = open(fsm.block_device_name, (int)fsm.flags, 0);
      assert(!iserrno(fsm.block_device_handle));
      fsm.block_device_number = 1;
      fsm.mount_name = ISO9660FS_MOUNTNAME_1;

      ercd = ioif_mount_device(fsm.mount_name, "B:", iso_fs_handle, (void *)&fsm);
      assert(!iserrno(ercd));

      /* register fixed media in USB drive as VFAT mounted on \usb2 */
      fsm.block_device_name = FAKEUSB_MOUNTNAME_2;
      fsm.flags = O_RDONLY;
      fsm.block_device_handle = open(fsm.block_device_name, (int)fsm.flags, 0);
      assert(!iserrno(fsm.block_device_handle));
      fsm.block_device_number = 2;
      fsm.mount_name = VFATFS_MOUNTNAME_2;

      ercd = ioif_mount_device(fsm.mount_name, "C:", vfat_fs_handle, (void *)&fsm);
      assert(!iserrno(ercd));

      /* register fixed media in CD drive as ISO mounted on \cdrom2 */
      fsm.block_device_name = FAKECD_MOUNTNAME_3;
      fsm.flags = O_RDONLY;
      fsm.block_device_handle = open(fsm.block_device_name, (int)fsm.flags, 0);
      assert(!iserrno(fsm.block_device_handle));
      fsm.block_device_number = 3;
      fsm.mount_name = ISO9660FS_MOUNTNAME_2;

      ercd = ioif_mount_device(fsm.mount_name, "D:", iso_fs_handle, (void *)&fsm);
      assert(!iserrno(ercd));

#   else
      /* use the automount so media can be changed and apps informed */

      /* VFAT config */
      static T_VFATFS_CONFIG vfatfs_cfg = {
         0                      /* not used - token gesture in case required later */
      };
      /* ISO config */
      static T_ISO9660FS_CONFIG isofs_cfg = {
         ISO9660_DEFAULT_BUFFER_SIZE,
         4                      /* support 2 CD devices */
      };
      /* various file systems */
      static AUTOMOUNT_FILESYSTEM_T filesys[2] = {
         /* name     FS API           config */
         {"VFAT", vfatfs_api, &vfatfs_cfg},     /* fs 0 */
         {"ISO9660", iso9660fs_api, &isofs_cfg} /* fs 1 */
      };
      /* filesystem list for automount */
      static AUTOMOUNT_FILESYSTEM_T *filesystems[] = {
         &filesys[0],
         &filesys[1],
         NULL
      };
      /* various dynamic mountpoints */
      static AUTOMOUNT_MOUNT_T mountlist[6] = {
         /* mount point            block device            fs  flags */
         {VFATFS_MOUNTNAME, SDHI_MOUNTNAME, 0, O_RDWR},
         {ISO9660FS_MOUNTNAME_1, FAKECD_MOUNTNAME_1, 1, O_RDONLY},
         {ISO9660FS_MOUNTNAME_2, FAKECD_MOUNTNAME_2, 1, O_RDONLY},
         {ISO9660FS_MOUNTNAME_3, FAKECD_MOUNTNAME_3, 1, O_RDONLY | AUTOMOUNT_FLAG_NOAUTO}
      };

      /* list of mounts for automount to maintain */
      static AUTOMOUNT_MOUNT_T *mounts[] = {
         &mountlist[0],         /* A: & 0 to/from library */
         NULL
      };

      ercd = automount_init(filesystems, mounts);
      assert(!iserrno(ercd));

#      ifdef TEST_AUTOMOUNT_NOTIFY
      ercd = automount_add_notify(test_block_notify);
#      endif
#   endif

   }

#endif

   /*
    *  Initialise mp3_codec_api
    */
#ifdef MP3_CODEC_API__CFG
   {
      MP3_CODEC_CONFIG_T mp3cdc_cfg = { MP3_TASK_PRIORITY };

      ercd = ioif_start_device(mp3codecif_api(), (void *)&mp3cdc_cfg, "cdc\\mp3_codec", "", NULL);
      assert(!iserrno(ercd));
   }
#endif

   /*
    *  Initialise wma_codec_api
    */
#ifdef WMA_CODEC_API__CFG
   {
      WMA_CODEC_CONFIG_T wmacdc_cfg = { WMA_TASK_PRIORITY };

      ercd = ioif_start_device(wmacodecif_api(), (void *)&wmacdc_cfg, "cdc\\wma_codec", "", NULL);
      assert(!iserrno(ercd));
   }
#endif

}

/**
 * @internal
 *
 * Support function for reentrant stdlib.
 *
 */
int *errno_addr(void)
{
   return (&(_errno));
}

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 20-aug-2008 kirk bailey
 * + Changed includes to be conditional based on configuration selections.
 *
 * 31-aug-2007 kirk bailey
 * + Added errno_addr from Frank Hohmann to support reentrant stdlib.
 *
 * 30-jul-2007 kirk bailey
 * + Got rid of C++ style comments.
 *
 * 04-May-2007 Dan Carman
 * + added start devices for additional Renesas Phase 7 peripherals
 *
 * 12-apr-2007 kirk bailey
 * + scif.h became drv_scif.h
 *
 * 30-jan-2007 kirk bailey
 * + Added configuration logic with conditional compilation. Added revision
 * + block and header block with SCM keywords.
 *
\*===========================================================================*/

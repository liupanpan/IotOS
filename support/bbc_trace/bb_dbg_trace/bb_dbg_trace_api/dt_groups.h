#ifndef DT_GROUPS_H
#   define DT_GROUPS_H
/*===========================================================================*/
/**
 * @file dt_groups.h
 *
 *   Maps the ISA debugtrace API to the BASA implementation.
 *
 * %full_filespec: dt_groups.h~2:incl:kok_inf#1 %
 * @version %version: 2 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Mon May 11 14:35:12 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM - Error Management
 *   - TR - TRace
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 *
 * @ingroup dbg_trace_client
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

#   define DT_SAL              ((uint64_t)1 <<  0)
#   define DT_MAIN             ((uint64_t)1 <<  1)
#   define DT_JSAL             ((uint64_t)1 <<  2)
#   define DT_LAYERMANAGER     ((uint64_t)1 <<  3)
#   define DT_DIAGNOSTICS      ((uint64_t)1 <<  4)
#   define DT_XML              ((uint64_t)1 <<  5)
#   define DT_POWERMODE        ((uint64_t)1 <<  6)
#   define DT_KEYPANEL         ((uint64_t)1 <<  7)
#   define DT_SWUPDATE         ((uint64_t)1 <<  8)
#   define DT_PERSISTENCE      ((uint64_t)1 <<  9)
#   define DT_AAF              ((uint64_t)1 << 10)
#   define DT_MEDIA            ((uint64_t)1 << 11)
#   define DT_MEDIA_SRV        ((uint64_t)1 << 12)
#   define DT_MEDIA_AFP        ((uint64_t)1 << 13)
#   define DT_MEDIA_FFT        ((uint64_t)1 << 14)
#   define DT_MEDIA_RES2       ((uint64_t)1 << 15)
#   define DT_MEDIA_RES3       ((uint64_t)1 << 16)
#   define DT_TUNER_AMFM       ((uint64_t)1 << 17)
#   define DT_TUNER_ANNOUNCE   ((uint64_t)1 << 18)
#   define DT_TUNER_STARTUP    ((uint64_t)1 << 19)
#   define DT_TUNER_DAB_APP    ((uint64_t)1 << 20)
#   define DT_TUNER_DAB_FE     ((uint64_t)1 << 21)
#   define DT_SPEECH           ((uint64_t)1 << 22)
#   define DT_TESTFRAMEWORK    ((uint64_t)1 << 23)
#   define DT_NAVTEST          ((uint64_t)1 << 24)
#   define DT_NAVCORE          ((uint64_t)1 << 25)
#   define DT_NAVEBA           ((uint64_t)1 << 26)
#   define DT_SPARE            ((uint64_t)1 << 27)
#   define DT_NAVVP            ((uint64_t)1 << 28)
#   define DT_NAVDS            ((uint64_t)1 << 29)
#   define DT_NAVMD            ((uint64_t)1 << 30)
#   define DT_NAVDG            ((uint64_t)1 << 31)
#   define DT_NAVIOM           ((uint64_t)1 << 32)
#   define DT_NAVTMC           ((uint64_t)1 << 33)
#   define DT_PERFORMANCE      ((uint64_t)1 << 34)
#   define DT_SENSORHANDLER    ((uint64_t)1 << 35)
#   define DT_MOST             ((uint64_t)1 << 36)
#   define DT_DSP              ((uint64_t)1 << 37)
#   define DT_VIP              ((uint64_t)1 << 38)
#   define DT_CAR_REF          ((uint64_t)1 << 39)
#   define DT_SYSTEMTIME       ((uint64_t)1 << 40)
#   define DT_TUNER_SDARS_DRV  ((uint64_t)1 << 41)
#   define DT_TUNER_SDARS_APP  ((uint64_t)1 << 42)
#   define DT_DSICPP           ((uint64_t)1 << 43)
#   define DT_AUDIOMANAGER     ((uint64_t)1 << 44)
#   define DT_MCR              ((uint64_t)1 << 45)
#   define DT_HMI_COMMON       ((uint64_t)1 << 46)
#   define DT_HMI_TUNER        ((uint64_t)1 << 47)
#   define DT_HMI_MEDIA        ((uint64_t)1 << 48)
#   define DT_HMI_AUDIO        ((uint64_t)1 << 49)
#   define DT_HMI_VOICE        ((uint64_t)1 << 50)
#   define DT_HMI_DIAGNOSTICS  ((uint64_t)1 << 51)

/* Group combinations: */
#   define DT_NO_GROUP   (uint64_t)0
#   define DT_ALL_GROUPS (uint64_t)-1

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

#define DT_GROUPS                                    \
{                                                    \
                                                     \
    { DT_SAL,               "DT_SAL"              }, \
    { DT_MAIN,              "DT_MAIN"             }, \
    { DT_JSAL,              "DT_JSAL"             }, \
    { DT_LAYERMANAGER,      "DT_LAYERMANAGER"     }, \
    { DT_DIAGNOSTICS,       "DT_DIAGNOSTICS"      }, \
    { DT_XML,               "DT_XML"              }, \
    { DT_POWERMODE,         "DT_POWERMODE"        }, \
    { DT_KEYPANEL,          "DT_KEYPANEL"         }, \
    { DT_SWUPDATE,          "DT_SWUPDATE"         }, \
    { DT_PERSISTENCE,       "DT_PERSISTENCE"      }, \
    { DT_AAF,               "DT_AAF"              }, \
    { DT_MEDIA,             "DT_MEDIA"            }, \
    { DT_MEDIA_SRV,         "DT_MEDIA_SRV"        }, \
    { DT_MEDIA_AFP,         "DT_MEDIA_AFP"        }, \
    { DT_MEDIA_FFT,         "DT_MEDIA_FFT"        }, \
    { DT_MEDIA_RES2,        "DT_MEDIA_RES2"       }, \
    { DT_MEDIA_RES3,        "DT_MEDIA_RES3"       }, \
    { DT_TUNER_AMFM,        "DT_TUNER_AMFM"       }, \
    { DT_TUNER_ANNOUNCE,    "DT_TUNER_ANNOUNCE"   }, \
    { DT_TUNER_STARTUP,     "DT_TUNER_STARTUP"    }, \
    { DT_TUNER_DAB_APP,     "DT_TUNER_DAB_APP"    }, \
    { DT_TUNER_DAB_FE,      "DT_TUNER_DAB_FE"     }, \
    { DT_SPEECH,            "DT_SPEECH"           }, \
    { DT_TESTFRAMEWORK,     "DT_TESTFRAMEWORK"    }, \
    { DT_NAVTEST,           "DT_NAVTEST"          }, \
    { DT_NAVCORE,           "DT_NAVCORE"          }, \
    { DT_NAVEBA,            "DT_NAVEBA"           }, \
    { DT_SPARE,             "DT_SPARE"            }, \
    { DT_NAVVP,             "DT_NAVVP"            }, \
    { DT_NAVDS,             "DT_NAVDS"            }, \
    { DT_NAVMD,             "DT_NAVMD"            }, \
    { DT_NAVDG,             "DT_NAVDG"            }, \
    { DT_NAVIOM,            "DT_NAVIOM"           }, \
    { DT_NAVTMC,            "DT_NAVTMC"           }, \
    { DT_PERFORMANCE,       "DT_PERFORMANCE"      }, \
    { DT_SENSORHANDLER,     "DT_SENSORHANDLER"    }, \
    { DT_MOST,              "DT_MOST"             }, \
    { DT_DSP,               "DT_DSP"              }, \
    { DT_VIP,               "DT_VIP"              }, \
    { DT_CAR_REF,           "DT_CAR_REF"          }, \
    { DT_SYSTEMTIME,        "DT_SYSTEMTIME"       }, \
    { DT_TUNER_SDARS_DRV,   "DT_TUNER_SDARS_DRV"  }, \
    { DT_TUNER_SDARS_APP,   "DT_TUNER_SDARS_APP"  }, \
    { DT_DSICPP,            "DT_DSICPP"           }, \
    { DT_AUDIOMANAGER,      "DT_AUDIOMANAGER"     }, \
    { DT_MCR,               "DT_MCR"              }, \
    { DT_HMI_COMMON,        "DT_HMI_COMMON"       }, \
    { DT_HMI_TUNER,         "DT_HMI_TUNER"        }, \
    { DT_HMI_MEDIA,         "DT_HMI_MEDIA"        }, \
    { DT_HMI_AUDIO,         "DT_HMI_AUDIO"        }, \
    { DT_HMI_VOICE,         "DT_HMI_VOICE"        }, \
                                                     \
    { DT_NO_GROUP,          "DT_NO_GROUP"         }, \
    { DT_ALL_GROUPS,        "DT_ALL_GROUPS"       }, \
}

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/* Symbolic names for the groups */
typedef struct DT_Group_Tag
{
      uint64_t value;
      const char* name;
}
DT_Group_T;

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/


#   ifdef __cplusplus
} /* extern "C" */
#   endif                    /* __cplusplus */
/*===========================================================================*/
/*!
 * @file dbg_trace.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 11-May-2015 Kirk Bailey
 *   - Created file from version 120_1428 of AP_DebugTrace
 *
 * - 11-May-2015 Kirk Bailey
 *   - Reformatted.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* DEBUGTRACE_H */

#ifndef TYPEDEFS_H
# define TYPEDEFS_H

# include <windows.h>

typedef enum _PS_ATTRIBUTE_NUM {
    PsAttributeParentProcess, // in HANDLE
    PsAttributeDebugPort, // in HANDLE
    PsAttributeToken, // in HANDLE
    PsAttributeClientId, // out PCLIENT_ID
    PsAttributeTebAddress, // out PTEB
    PsAttributeImageName, // in PWSTR
    PsAttributeImageInfo, // out PSECTION_IMAGE_INFORMATION
    PsAttributeMemoryReserve, // in PPS_MEMORY_RESERVE
    PsAttributePriorityClass, // in UCHAR
    PsAttributeErrorMode, // in ULONG
    PsAttributeStdHandleInfo, // 10, in PPS_STD_HANDLE_INFO
    PsAttributeHandleList, // in PHANDLE
    PsAttributeGroupAffinity, // in PGROUP_AFFINITY
    PsAttributePreferredNode, // in PUSHORT
    PsAttributeIdealProcessor, // in PPROCESSOR_NUMBER
    PsAttributeUmsThread, // see UpdateProceThreadAttributeList in msdn (CreateProcessA/W...) in PUMS_CREATE_THREAD_ATTRIBUTES
    PsAttributeMitigationOptions, // in UCHAR
    PsAttributeProtectionLevel,
    PsAttributeSecureProcess, // since THRESHOLD (Virtual Secure Mode, Device Guard)
    PsAttributeJobList,
    PsAttributeMax
} PS_ATTRIBUTE_NUM;

# define RTL_USER_PROCESS_PARAMETERS_NORMALIZED 0x01
# define PS_ATTRIBUTE_NUMBER_MASK    0x0000ffff
# define PS_ATTRIBUTE_THREAD         0x00010000
# define PS_ATTRIBUTE_INPUT          0x00020000
# define PS_ATTRIBUTE_ADDITIVE       0x00040000
# define PsAttributeValue(Number, Thread, Input, Additive) \
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK) | \
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0) | \
    ((Input) ? PS_ATTRIBUTE_INPUT : 0) | \
    ((Additive) ? PS_ATTRIBUTE_ADDITIVE : 0))
# define PS_ATTRIBUTE_IMAGE_INFO \
    PsAttributeValue(PsAttributeImageInfo, FALSE, FALSE, FALSE) // 0x6
# define PS_ATTRIBUTE_IMAGE_NAME \
    PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE) // 0x20005
#define PS_ATTRIBUTE_CLIENT_ID \
    PsAttributeValue(PsAttributeClientId, TRUE, FALSE, FALSE) // 0x10003
#define PS_ATTRIBUTE_STD_HANDLE_INFO \
    PsAttributeValue(PsAttributeStdHandleInfo, FALSE, TRUE, FALSE) // 0x2000A
#define PS_ATTRIBUTE_MITIGATION_OPTIONS \
    PsAttributeValue(PsAttributeMitigationOptions, FALSE, TRUE, FALSE) // 0x60010
# define    PROCESS_CREATE_FLAGS_INHERIT_HANDLES   0x00000004

/*
* Define custom structs
*/
typedef struct _SYSCALL_ENTRY {
    PVOID Address;
    unsigned long Hash;
    SIZE_T Size;
} SYSCALL_ENTRY,
* PSYSCALL_ENTRY;

typedef struct _LSA_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} LSA_UNICODE_STRING, * PLSA_UNICODE_STRING, UNICODE_STRING, * PUNICODE_STRING, * PUNICODE_STR;
typedef const UNICODE_STRING* PCUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} OBJECT_ATTRIBUTES,
* POBJECT_ATTRIBUTES;
typedef const OBJECT_ATTRIBUTES* PCOBJECT_ATTRIBUTES;

typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID    Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef struct _PS_PROTECTION {
    union {
        UCHAR Level;
        struct {
            UCHAR Type : 3;
            UCHAR Audit : 1;                  // Reserved
            UCHAR Signer : 4;
        };
    };
} PS_PROTECTION, * PPS_PROTECTION;

typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, * PCURDIR;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
    USHORT Flags;

    USHORT Length;
    ULONG TimeStamp;
    UNICODE_STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, * PRTL_DRIVE_LETTER_CURDIR;

# define RTL_MAX_DRIVE_LETTERS 32
typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PWCHAR Environment;

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

    ULONG_PTR EnvironmentSize;
    ULONG_PTR EnvironmentVersion;
    PVOID PackageDependencyData;
    ULONG ProcessGroupId;
    ULONG LoaderThreads;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA {
    ULONG                   Length;
    ULONG                   Initialized;
    PVOID                   SsHandle;
    LIST_ENTRY              InLoadOrderModuleList;
    LIST_ENTRY              InMemoryOrderModuleList;
    LIST_ENTRY              InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
    BOOLEAN                 InheritedAddressSpace;
    BOOLEAN                 ReadImageFileExecOptions;
    BOOLEAN                 BeingDebugged;
    BOOLEAN                 Spare;
    HANDLE                  Mutant;
    PVOID                   ImageBase;
    PPEB_LDR_DATA           LoaderData;
    PRTL_USER_PROCESS_PARAMETERS                   ProcessParameters;
    PVOID                   SubSystemData;
    PVOID                   ProcessHeap;
    PVOID                   FastPebLock;
    PVOID                   FastPebLockRoutine;
    PVOID                   FastPebUnlockRoutine;
    ULONG                   EnvironmentUpdateCount;
    PVOID* KernelCallbackTable;
    PVOID                   EventLogSection;
    PVOID                   EventLog;
    PVOID                   FreeList;
    ULONG                   TlsExpansionCounter;
    PVOID                   TlsBitmap;
    ULONG                   TlsBitmapBits[0x2];
    PVOID                   ReadOnlySharedMemoryBase;
    PVOID                   ReadOnlySharedMemoryHeap;
    PVOID* ReadOnlyStaticServerData;
    PVOID                   AnsiCodePageData;
    PVOID                   OemCodePageData;
    PVOID                   UnicodeCaseTableData;
    ULONG                   NumberOfProcessors;
    ULONG                   NtGlobalFlag;
    BYTE                    Spare2[0x4];
    LARGE_INTEGER           CriticalSectionTimeout;
    ULONG                   HeapSegmentReserve;
    ULONG                   HeapSegmentCommit;
    ULONG                   HeapDeCommitTotalFreeThreshold;
    ULONG                   HeapDeCommitFreeBlockThreshold;
    ULONG                   NumberOfHeaps;
    ULONG                   MaximumNumberOfHeaps;
    PVOID** ProcessHeaps;
    PVOID                   GdiSharedHandleTable;
    PVOID                   ProcessStarterHelper;
    PVOID                   GdiDCAttributeList;
    PVOID                   LoaderLock;
    ULONG                   OSMajorVersion;
    ULONG                   OSMinorVersion;
    ULONG                   OSBuildNumber;
    ULONG                   OSPlatformId;
    ULONG                   ImageSubSystem;
    ULONG                   ImageSubSystemMajorVersion;
    ULONG                   ImageSubSystemMinorVersion;
    ULONG                   GdiHandleBuffer[0x22];
    ULONG                   PostProcessInitRoutine;
    ULONG                   TlsExpansionBitmap;
    BYTE                    TlsExpansionBitmapBits[0x80];
    ULONG                   SessionId;
} PEB, * PPEB;

typedef struct __CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, * PCLIENT_ID;

typedef PVOID PACTIVATION_CONTEXT;
typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME {
    struct __RTL_ACTIVATION_CONTEXT_STACK_FRAME* Previous;
    PACTIVATION_CONTEXT ActivationContext;
    ULONG Flags;
} RTL_ACTIVATION_CONTEXT_STACK_FRAME, * PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _ACTIVATION_CONTEXT_STACK {
    PRTL_ACTIVATION_CONTEXT_STACK_FRAME ActiveFrame;
    LIST_ENTRY FrameListCache;
    ULONG Flags;
    ULONG NextCookieSequenceNumber;
    ULONG StackId;
} ACTIVATION_CONTEXT_STACK, * PACTIVATION_CONTEXT_STACK;

typedef struct _GDI_TEB_BATCH {
    ULONG Offset;
    ULONG HDC;
    ULONG Buffer[310];
} GDI_TEB_BATCH, * PGDI_TEB_BATCH;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT {
    ULONG Flags;
    PCHAR FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, * PTEB_ACTIVE_FRAME_CONTEXT;

typedef struct _TEB_ACTIVE_FRAME {
    ULONG Flags;
    struct _TEB_ACTIVE_FRAME* Previous;
    PTEB_ACTIVE_FRAME_CONTEXT Context;
} TEB_ACTIVE_FRAME, * PTEB_ACTIVE_FRAME;

typedef struct _TEB {
    NT_TIB              NtTib;
    PVOID               EnvironmentPointer;
    CLIENT_ID           ClientId;
    PVOID               ActiveRpcHandle;
    PVOID               ThreadLocalStoragePointer;
    PPEB                ProcessEnvironmentBlock;
    ULONG               LastErrorValue;
    ULONG               CountOfOwnedCriticalSections;
    PVOID               CsrClientThread;
    PVOID               Win32ThreadInfo;
    ULONG               User32Reserved[26];
    ULONG               UserReserved[5];
    PVOID               WOW32Reserved;
    LCID                CurrentLocale;
    ULONG               FpSoftwareStatusRegister;
    PVOID               SystemReserved1[54];
    LONG                ExceptionCode;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    PACTIVATION_CONTEXT_STACK* ActivationContextStackPointer;
    UCHAR                  SpareBytes1[0x30 - 3 * sizeof(PVOID)];
    ULONG                  TxFsContext;
#elif (NTDDI_VERSION >= NTDDI_WS03)
    PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;
    UCHAR                  SpareBytes1[0x34 - 3 * sizeof(PVOID)];
#else
    ACTIVATION_CONTEXT_STACK ActivationContextStack;
    UCHAR                  SpareBytes1[24];
#endif
    GDI_TEB_BATCH           GdiTebBatch;
    CLIENT_ID               RealClientId;
    PVOID                   GdiCachedProcessHandle;
    ULONG                   GdiClientPID;
    ULONG                   GdiClientTID;
    PVOID                   GdiThreadLocalInfo;
    PSIZE_T                 Win32ClientInfo[62];
    PVOID                   glDispatchTable[233];
    PSIZE_T                 glReserved1[29];
    PVOID                   glReserved2;
    PVOID                   glSectionInfo;
    PVOID                   glSection;
    PVOID                   glTable;
    PVOID                   glCurrentRC;
    PVOID                   glContext;
    NTSTATUS                LastStatusValue;
    UNICODE_STRING          StaticUnicodeString;
    WCHAR                   StaticUnicodeBuffer[261];
    PVOID                   DeallocationStack;
    PVOID                   TlsSlots[64];
    LIST_ENTRY              TlsLinks;
    PVOID                   Vdm;
    PVOID                   ReservedForNtRpc;
    PVOID                   DbgSsReserved[2];
#if (NTDDI_VERSION >= NTDDI_WS03)
    ULONG                   HardErrorMode;
#else
    ULONG                  HardErrorsAreDisabled;
#endif
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    PVOID                   Instrumentation[13 - sizeof(GUID) / sizeof(PVOID)];
    GUID                    ActivityId;
    PVOID                   SubProcessTag;
    PVOID                   EtwLocalData;
    PVOID                   EtwTraceData;
#elif (NTDDI_VERSION >= NTDDI_WS03)
    PVOID                   Instrumentation[14];
    PVOID                   SubProcessTag;
    PVOID                   EtwLocalData;
#else
    PVOID                   Instrumentation[16];
#endif
    PVOID                   WinSockData;
    ULONG                   GdiBatchCount;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    BOOLEAN                SpareBool0;
    BOOLEAN                SpareBool1;
    BOOLEAN                SpareBool2;
#else
    BOOLEAN                InDbgPrint;
    BOOLEAN                FreeStackOnTermination;
    BOOLEAN                HasFiberData;
#endif
    UCHAR                  IdealProcessor;
#if (NTDDI_VERSION >= NTDDI_WS03)
    ULONG                  GuaranteedStackBytes;
#else
    ULONG                  Spare3;
#endif
    PVOID                  ReservedForPerf;
    PVOID                  ReservedForOle;
    ULONG                  WaitingOnLoaderLock;
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    PVOID                  SavedPriorityState;
    ULONG_PTR              SoftPatchPtr1;
    ULONG_PTR              ThreadPoolData;
#elif (NTDDI_VERSION >= NTDDI_WS03)
    ULONG_PTR              SparePointer1;
    ULONG_PTR              SoftPatchPtr1;
    ULONG_PTR              SoftPatchPtr2;
#else
    Wx86ThreadState        Wx86Thread;
#endif
    PVOID* TlsExpansionSlots;
#if defined(_WIN64) && !defined(EXPLICIT_32BIT)
    PVOID                  DeallocationBStore;
    PVOID                  BStoreLimit;
#endif
    ULONG                  ImpersonationLocale;
    ULONG                  IsImpersonating;
    PVOID                  NlsCache;
    PVOID                  pShimData;
    ULONG                  HeapVirtualAffinity;
    HANDLE                 CurrentTransactionHandle;
    PTEB_ACTIVE_FRAME      ActiveFrame;
#if (NTDDI_VERSION >= NTDDI_WS03)
    PVOID FlsData;
#endif
#if (NTDDI_VERSION >= NTDDI_LONGHORN)
    PVOID PreferredLangauges;
    PVOID UserPrefLanguages;
    PVOID MergedPrefLanguages;
    ULONG MuiImpersonation;
    union
    {
        struct
        {
            USHORT SpareCrossTebFlags : 16;
        };
        USHORT CrossTebFlags;
    };
    union
    {
        struct
        {
            USHORT DbgSafeThunkCall : 1;
            USHORT DbgInDebugPrint : 1;
            USHORT DbgHasFiberData : 1;
            USHORT DbgSkipThreadAttach : 1;
            USHORT DbgWerInShipAssertCode : 1;
            USHORT DbgIssuedInitialBp : 1;
            USHORT DbgClonedThread : 1;
            USHORT SpareSameTebBits : 9;
        };
        USHORT SameTebFlags;
    };
    PVOID TxnScopeEntercallback;
    PVOID TxnScopeExitCAllback;
    PVOID TxnScopeContext;
    ULONG LockCount;
    ULONG ProcessRundown;
    ULONG64 LastSwitchTime;
    ULONG64 TotalSwitchOutTime;
    LARGE_INTEGER WaitReasonBitMap;
#else
    BOOLEAN SafeThunkCall;
    BOOLEAN BooleanSpare[3];
#endif
} TEB, * PTEB;

typedef _Function_class_(IO_APC_ROUTINE)
VOID NTAPI IO_APC_ROUTINE(
    _In_ PVOID ApcContext,
    _In_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG Reserved
);
typedef IO_APC_ROUTINE* PIO_APC_ROUTINE;

typedef enum _PS_CREATE_STATE {
    PsCreateInitialState,
    PsCreateFailOnFileOpen,
    PsCreateFailOnSectionCreate,
    PsCreateFailExeFormat,
    PsCreateFailMachineMismatch,
    PsCreateFailExeName, // Debugger specified
    PsCreateSuccess,
    PsCreateMaximumStates
} PS_CREATE_STATE;

typedef struct _PS_CREATE_INFO {
    SIZE_T Size;
    PS_CREATE_STATE State;
    union {
        // PsCreateInitialState
        struct {
            union {
                ULONG InitFlags;
                struct {
                    UCHAR WriteOutputOnExit : 1;
                    UCHAR DetectManifest : 1;
                    UCHAR IFEOSkipDebugger : 1;
                    UCHAR IFEODoNotPropagateKeyState : 1;
                    UCHAR SpareBits1 : 4;
                    UCHAR SpareBits2 : 8;
                    USHORT ProhibitedImageCharacteristics : 16;
                };
            };
            ACCESS_MASK AdditionalFileAccess;
        } InitState;

        // PsCreateFailOnSectionCreate
        struct {
            HANDLE FileHandle;
        } FailSection;

        // PsCreateFailExeFormat
        struct {
            USHORT DllCharacteristics;
        } ExeFormat;

        // PsCreateFailExeName
        struct {
            HANDLE IFEOKey;
        } ExeName;

        // PsCreateSuccess
        struct {
            union {
                ULONG OutputFlags;
                struct {
                    UCHAR ProtectedProcess : 1;
                    UCHAR AddressSpaceOverride : 1;
                    UCHAR DevOverrideEnabled : 1; // from Image File Execution Options
                    UCHAR ManifestDetected : 1;
                    UCHAR ProtectedProcessLight : 1;
                    UCHAR SpareBits1 : 3;
                    UCHAR SpareBits2 : 8;
                    USHORT SpareBits3 : 16;
                };
            };
            HANDLE FileHandle;
            HANDLE SectionHandle;
            ULONGLONG UserProcessParametersNative;
            ULONG UserProcessParametersWow64;
            ULONG CurrentParameterFlags;
            ULONGLONG PebAddressNative;
            ULONG PebAddressWow64;
            ULONGLONG ManifestAddress;
            ULONG ManifestSize;
        } SuccessState;
    };
} PS_CREATE_INFO, * PPS_CREATE_INFO;

typedef enum _WINDOW_FLAGS
{
    WF_HIDE_WINDOW = 0x1,
    WF_HIDE_TEXT = 0x10,
    WF_REDIRECT_HANDLES = 0x100
} WINDOW_FLAGS;

typedef struct _PS_STD_HANDLE_INFO
{
    union
    {
        ULONG Flags;
        struct
        {
            ULONG StdHandleState : 2; // PS_STD_HANDLE_STATE
            ULONG PseudoHandleMask : 3; // PS_STD_*
        } s;
    };
    ULONG StdHandleSubsystemType;
} PS_STD_HANDLE_INFO, * PPS_STD_HANDLE_INFO;

typedef struct _PS_ATTRIBUTE
{
    ULONG_PTR Attribute;                // PROC_THREAD_ATTRIBUTE_XXX | PROC_THREAD_ATTRIBUTE_XXX modifiers, see ProcThreadAttributeValue macro and Windows Internals 6 (372)
    SIZE_T Size;                        // Size of Value or *ValuePtr
    union
    {
        ULONG_PTR Value;                // Reserve 8 bytes for data (such as a Handle or a data pointer)
        PVOID ValuePtr;                 // data pointer
    };
    PSIZE_T ReturnLength;               // Either 0 or specifies size of data returned to caller via "ValuePtr"
} PS_ATTRIBUTE, * PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST
{
    SIZE_T TotalLength;                 // sizeof(PS_ATTRIBUTE_LIST)
    PS_ATTRIBUTE Attributes[6];         // Depends on how many attribute entries should be supplied to NtCreateUserProcess
} PS_ATTRIBUTE_LIST, * PPS_ATTRIBUTE_LIST;

typedef struct _SECTION_IMAGE_INFORMATION
{
    PVOID TransferAddress; // Entry point
    ULONG ZeroBits;
    SIZE_T MaximumStackSize;
    SIZE_T CommittedStackSize;
    ULONG SubSystemType;
    union
    {
        struct
        {
            USHORT SubSystemMinorVersion;
            USHORT SubSystemMajorVersion;
        } s1;
        ULONG SubSystemVersion;
    } u1;
    union
    {
        struct
        {
            USHORT MajorOperatingSystemVersion;
            USHORT MinorOperatingSystemVersion;
        } s2;
        ULONG OperatingSystemVersion;
    } u2;
    USHORT ImageCharacteristics;
    USHORT DllCharacteristics;
    USHORT Machine;
    BOOLEAN ImageContainsCode;
    union
    {
        UCHAR ImageFlags;
        struct
        {
            UCHAR ComPlusNativeReady : 1;
            UCHAR ComPlusILOnly : 1;
            UCHAR ImageDynamicallyRelocated : 1;
            UCHAR ImageMappedFlat : 1;
            UCHAR BaseBelow4gb : 1;
            UCHAR ComPlusPrefer32bit : 1;
            UCHAR Reserved : 2;
        } s3;
    } u3;
    ULONG LoaderFlags;
    ULONG ImageFileSize;
    ULONG CheckSum;
} SECTION_IMAGE_INFORMATION, * PSECTION_IMAGE_INFORMATION;

//
// Definitions for Object Creation
//

#define OBJ_INHERIT                             0x00000002L
#define OBJ_PERMANENT                           0x00000010L
#define OBJ_EXCLUSIVE                           0x00000020L
#define OBJ_CASE_INSENSITIVE                    0x00000040L
#define OBJ_OPENIF                              0x00000080L
#define OBJ_OPENLINK                            0x00000100L
#define OBJ_KERNEL_HANDLE                       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK                  0x00000400L
#define OBJ_VALID_ATTRIBUTES                    0x000007F2L

//-----------------------------------------------------------------------------
// Object functions

//
// Object Manager Directory Specific Access Rights.
//
#define DIRECTORY_QUERY                         0x0001
#define DIRECTORY_TRAVERSE                      0x0002
#define DIRECTORY_CREATE_OBJECT                 0x0004
#define DIRECTORY_CREATE_SUBDIRECTORY           0x0008
#define DIRECTORY_ALL_ACCESS                    (STANDARD_RIGHTS_REQUIRED | 0xF)

#endif


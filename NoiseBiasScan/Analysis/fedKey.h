#ifndef _fedkey_h_
#define _fedkey_h_
typedef unsigned int fedKeyType;

// Definition of bit field positions for 32-bit key 
#define fedCrateOffset_  24
#define fedSlotOffset_   19
#define fedIdOffset_     10
#define feUnitOffset_     6
#define feChanOffset_     2
#define fedApvOffset_     0

// Definition of bit field masks for 32-bit key 
#define fedCrateMask_ 0x03F
#define fedSlotMask_  0x01F
#define fedIdMask_    0x1FF
#define feUnitMask_   0x00F
#define feChanMask_   0x00F
#define fedApvMask_   0x003

fedKeyType getFedCrate(fedKeyType x) { return  ((fedKeyType)((x >> fedCrateOffset_) & fedCrateMask_)) ;};
fedKeyType getFedSlot(fedKeyType x) { return  ((fedKeyType)((x >> fedSlotOffset_)  & fedSlotMask_ )) ;};
fedKeyType getFedId(fedKeyType x) { return  ((fedKeyType)((x >> fedIdOffset_)    & fedIdMask_   )) ;};
fedKeyType getFeUnit(fedKeyType x) { return  ((fedKeyType)((x >> feUnitOffset_)   & feUnitMask_  )) ;};
fedKeyType getFeChannel(fedKeyType x) { return  ((fedKeyType)((x >> feChanOffset_)   & feChanMask_  )) ;};
fedKeyType getApv(fedKeyType x) { return  ((fedKeyType)((x >> fedApvOffset_)   & fedApvMask_  )) ;};

fedKeyType setFedCrate(fedKeyType x) { return  ((fedKeyType)((x & fedCrateMask_) << fedCrateOffset_ )) ;};
fedKeyType setFedSlot(fedKeyType x) { return  ((fedKeyType)((x & fedSlotMask_)  << fedSlotOffset_  )) ;};
fedKeyType setFedId(fedKeyType x) { return  ((fedKeyType)((x & fedIdMask_)    << fedIdOffset_    )) ;};
fedKeyType setFeUnit(fedKeyType x) { return  ((fedKeyType)((x & feUnitMask_)   << feUnitOffset_   )) ;};
fedKeyType setFeChannel(fedKeyType x) { return  ((fedKeyType)((x & feChanMask_)   << feChanOffset_   )) ;};
fedKeyType setApv(fedKeyType x) { return  ((fedKeyType)((x & fedApvMask_)   << fedApvOffset_   )) ;};

#endif

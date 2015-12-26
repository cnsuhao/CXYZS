LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := raknet_satic

LOCAL_MODULE_FILENAME := raknet

LOCAL_SRC_FILES := \
VitaIncludes.cpp \
BitStream.cpp \
VariadicSQLParser.cpp \
VariableListDeltaTracker.cpp \
CCRakNetSlidingWindow.cpp \
VariableDeltaSerializer.cpp \
CCRakNetUDT.cpp \
UDPProxyServer.cpp \
CheckSum.cpp \
UDPProxyCoordinator.cpp \
CloudClient.cpp \
UDPProxyClient.cpp \
CloudCommon.cpp \
UDPForwarder.cpp \
CloudServer.cpp \
TwoWayAuthentication.cpp \
ThreadsafePacketLogger.cpp \
CommandParserInterface.cpp \
TelnetTransport.cpp \
TeamManager.cpp \
ConnectionGraph2.cpp \
TeamBalancer.cpp \
ConsoleServer.cpp \
TCPInterface.cpp \
DataCompressor.cpp \
TableSerializer.cpp \
DirectoryDeltaTransfer.cpp \
SuperFastHash.cpp \
StringTable.cpp \
StringCompressor.cpp \
DS_BytePool.cpp \
DS_ByteQueue.cpp \
StatisticsHistory.cpp \
SocketLayer.cpp \
SimpleMutex.cpp \
DS_HuffmanEncodingTree.cpp \
SignaledEvent.cpp \
SHA1.cpp \
SendToThread.cpp \
SecureHandshake.cpp \
RPC4Plugin.cpp \
Router2.cpp \
ReplicaManager3.cpp \
ReliabilityLayer.cpp \
ReadyEvent.cpp \
WSAStartupSingleton.cpp \
Rand.cpp \
RakWString.cpp \
RakThread.cpp \
DS_Table.cpp \
RakString.cpp \
RakSleep.cpp \
RakPeer.cpp \
RakNetTypes.cpp \
DynDNS.cpp \
RakNetTransport2.cpp \
EmailSender.cpp \
RakNetStatistics.cpp \
EncodeClassName.cpp \
EpochTimeToString.cpp \
RakNetSocket.cpp \
RakNetCommandParser.cpp \
FileList.cpp \
RakMemoryOverride.cpp \
FileListTransfer.cpp \
Rackspace.cpp \
PluginInterface2.cpp \
FileOperations.cpp \
PacketOutputWindowLogger.cpp \
FormatString.cpp \
PacketLogger.cpp \
FullyConnectedMesh2.cpp \
PacketizedTCP.cpp \
Getche.cpp \
PacketFileLogger.cpp \
Gets.cpp \
PacketConsoleLogger.cpp \
GetTime.cpp \
NetworkIDObject.cpp \
gettimeofday.cpp \
NetworkIDManager.cpp \
GridSectorizer.cpp \
NatTypeDetectionServer.cpp \
HTTPConnection.cpp \
NatTypeDetectionCommon.cpp \
IncrementalReadInterface.cpp \
NatTypeDetectionClient.cpp \
NatPunchthroughServer.cpp \
Itoa.cpp \
NatPunchthroughClient.cpp \
MessageFilter.cpp \
LinuxStrings.cpp \
_FindFirst.cpp \
LocklessTypes.cpp \
rdlmalloc.cpp \
LogCommandParser.cpp

LOCAL_CFLAGS := -Wno-c++11-narrowing -Wno-reserved-user-defined-literal

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/..

LOCAL_C_INCLUDES := $(LOCAL_PATH)/..

include $(BUILD_STATIC_LIBRARY)

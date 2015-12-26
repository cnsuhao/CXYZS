LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../ServerProject)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

FILE_LIST := hellocpp/main.cpp  
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)   
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Base/*.cpp)   
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Logic/*.cpp)   
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Net/*.cpp)  
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Manager/*.cpp)    
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/UI/*.cpp)  
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Effect/*.cpp)  
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../ServerProject/Common/Logic/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../ServerProject/Common/Tool/CSVParser.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%) 


LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/../../Classes/Base \
$(LOCAL_PATH)/../../Classes/Logic \
$(LOCAL_PATH)/../../Classes/Manager \
$(LOCAL_PATH)/../../Classes/Net \
$(LOCAL_PATH)/../../Classes/UI \
$(LOCAL_PATH)/../../Classes/Effect \
$(LOCAL_PATH)/../../../ServerProject\Common\Define \
$(LOCAL_PATH)/../../../ServerProject\Common\Logic \
$(LOCAL_PATH)/../../../ServerProject\Common\Tool \
$(LOCAL_PATH)/../../../ServerProject\RakNet \
$(LOCAL_PATH)/../../cocos2d \
$(LOCAL_PATH)/../../cocos2d/external/json \
$(LOCAL_PATH)/../../cocos2d/cocos/platform \
$(LOCAL_PATH)/../../cocos2d/extensions \
$(LOCAL_PATH)/../../cocos2d/cocos/editor-support \
$(LOCAL_PATH)/../../cocos2d/cocos/editor-support/cocostudio \
$(LOCAL_PATH)/../../cocos2d/cocos/ui \
$(LOCAL_PATH)/../../cocos2d/cocos/2d \
$(LOCAL_PATH)/../../cocos2d/cocos/renderer \
$(LOCAL_PATH)/../../cocos2d/cocos/network \

LOCAL_STATIC_LIBRARIES := cocos2dx_static
LOCAL_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_STATIC_LIBRARIES += cocos_extension_static
LOCAL_STATIC_LIBRARIES += cocos_gui_static
LOCAL_STATIC_LIBRARIES += cocostudio_static
LOCAL_STATIC_LIBRARIES += cocos_network_static
LOCAL_STATIC_LIBRARIES += iconv_static
LOCAL_STATIC_LIBRARIES += raknet_satic

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)
$(call import-module,extensions)
$(call import-module,network)
$(call import-module,ui)
$(call import-module,editor-support/cocostudio)
$(call import-module,iconv)
$(call import-module,raknet)
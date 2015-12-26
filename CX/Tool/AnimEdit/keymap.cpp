#include <unordered_map>
#include "cocos2d.h"

USING_NS_CC;

std::unordered_map<int, EventKeyboard::KeyCode> g_EditorKeyCodeMap;


struct EditorKeyCodeItem
{
    int windows_vk;
    EventKeyboard::KeyCode cocos_keycode;
};

static EditorKeyCodeItem g_EditorKeyCodeStructArray[] = {
    /* The unknown key */
    { GLFW_KEY_UNKNOWN         , EventKeyboard::KeyCode::KEY_NONE          },

    /* Printable keys */
    { VK_SPACE           , EventKeyboard::KeyCode::KEY_SPACE         },
    { GLFW_KEY_APOSTROPHE      , EventKeyboard::KeyCode::KEY_APOSTROPHE    },
    { GLFW_KEY_COMMA           , EventKeyboard::KeyCode::KEY_COMMA         },
    { GLFW_KEY_MINUS           , EventKeyboard::KeyCode::KEY_MINUS         },
    { GLFW_KEY_PERIOD          , EventKeyboard::KeyCode::KEY_PERIOD        },
    { GLFW_KEY_SLASH           , EventKeyboard::KeyCode::KEY_SLASH         },
    { VK_NUMPAD0               , EventKeyboard::KeyCode::KEY_0             },
    { VK_NUMPAD1               , EventKeyboard::KeyCode::KEY_1             },
    { VK_NUMPAD2               , EventKeyboard::KeyCode::KEY_2             },
    { VK_NUMPAD3               , EventKeyboard::KeyCode::KEY_3             },
    { VK_NUMPAD4               , EventKeyboard::KeyCode::KEY_4             },
    { VK_NUMPAD5               , EventKeyboard::KeyCode::KEY_5             },
    { VK_NUMPAD6               , EventKeyboard::KeyCode::KEY_6             },
    { VK_NUMPAD7               , EventKeyboard::KeyCode::KEY_7             },
    { VK_NUMPAD8               , EventKeyboard::KeyCode::KEY_8             },
    { VK_NUMPAD9               , EventKeyboard::KeyCode::KEY_9             },
    { GLFW_KEY_SEMICOLON       , EventKeyboard::KeyCode::KEY_SEMICOLON     },
    { GLFW_KEY_EQUAL           , EventKeyboard::KeyCode::KEY_EQUAL         },
    { 'A'               , EventKeyboard::KeyCode::KEY_A             },
    { 'B'               , EventKeyboard::KeyCode::KEY_B             },
    { 'C'               , EventKeyboard::KeyCode::KEY_C             },
    { 'D'               , EventKeyboard::KeyCode::KEY_D             },
    { 'E'               , EventKeyboard::KeyCode::KEY_E             },
    { 'F'               , EventKeyboard::KeyCode::KEY_F             },
    { 'G'               , EventKeyboard::KeyCode::KEY_G             },
    { 'H'               , EventKeyboard::KeyCode::KEY_H             },
    { 'I'               , EventKeyboard::KeyCode::KEY_I             },
    { 'J'               , EventKeyboard::KeyCode::KEY_J             },
    { 'K'               , EventKeyboard::KeyCode::KEY_K             },
    { 'L'               , EventKeyboard::KeyCode::KEY_L             },
    { 'M'               , EventKeyboard::KeyCode::KEY_M             },
    { 'N'               , EventKeyboard::KeyCode::KEY_N             },
    { 'O'               , EventKeyboard::KeyCode::KEY_O             },
    { 'P'               , EventKeyboard::KeyCode::KEY_P             },
    { 'Q'               , EventKeyboard::KeyCode::KEY_Q             },
    { 'R'               , EventKeyboard::KeyCode::KEY_R             },
    { 'S'               , EventKeyboard::KeyCode::KEY_S             },
    { 'T'               , EventKeyboard::KeyCode::KEY_T             },
    { 'U'               , EventKeyboard::KeyCode::KEY_U             },
    { 'V'               , EventKeyboard::KeyCode::KEY_V             },
    { 'W'               , EventKeyboard::KeyCode::KEY_W             },
    { 'X'               , EventKeyboard::KeyCode::KEY_X             },
    { 'Y'               , EventKeyboard::KeyCode::KEY_Y             },
    { 'Z'               , EventKeyboard::KeyCode::KEY_Z             },
    { GLFW_KEY_LEFT_BRACKET    , EventKeyboard::KeyCode::KEY_LEFT_BRACKET  },
    { GLFW_KEY_BACKSLASH       , EventKeyboard::KeyCode::KEY_BACK_SLASH    },
    { GLFW_KEY_RIGHT_BRACKET   , EventKeyboard::KeyCode::KEY_RIGHT_BRACKET },
    { GLFW_KEY_GRAVE_ACCENT    , EventKeyboard::KeyCode::KEY_GRAVE         },
    { GLFW_KEY_WORLD_1         , EventKeyboard::KeyCode::KEY_GRAVE         },
    { GLFW_KEY_WORLD_2         , EventKeyboard::KeyCode::KEY_NONE          },

    /* Function keys */
    { VK_ESCAPE          , EventKeyboard::KeyCode::KEY_ESCAPE        },
    { VK_RETURN           , EventKeyboard::KeyCode::KEY_KP_ENTER      },
    { VK_TAB             , EventKeyboard::KeyCode::KEY_TAB           },
    { VK_BACK       , EventKeyboard::KeyCode::KEY_BACKSPACE     },
    { VK_INSERT          , EventKeyboard::KeyCode::KEY_INSERT        },
    { VK_DELETE          , EventKeyboard::KeyCode::KEY_DELETE        },
    { VK_RIGHT           , EventKeyboard::KeyCode::KEY_RIGHT_ARROW   },
    { VK_LEFT            , EventKeyboard::KeyCode::KEY_LEFT_ARROW    },
    { VK_DOWN            , EventKeyboard::KeyCode::KEY_DOWN_ARROW    },
    { VK_UP              , EventKeyboard::KeyCode::KEY_UP_ARROW      },
    { VK_PRIOR         , EventKeyboard::KeyCode::KEY_KP_PG_UP      },
    { VK_NEXT       , EventKeyboard::KeyCode::KEY_KP_PG_DOWN    },
    { VK_HOME            , EventKeyboard::KeyCode::KEY_KP_HOME       },
    { VK_END             , EventKeyboard::KeyCode::KEY_END           },
    { VK_CAPITAL       , EventKeyboard::KeyCode::KEY_CAPS_LOCK     },
    { VK_SCROLL     , EventKeyboard::KeyCode::KEY_SCROLL_LOCK   },
    { VK_NUMLOCK        , EventKeyboard::KeyCode::KEY_NUM_LOCK      },
    { VK_PRINT    , EventKeyboard::KeyCode::KEY_PRINT         },
    { VK_PAUSE           , EventKeyboard::KeyCode::KEY_PAUSE         },
    { VK_F1                    , EventKeyboard::KeyCode::KEY_F1            },
    { VK_F2              , EventKeyboard::KeyCode::KEY_F2            },
    { VK_F3              , EventKeyboard::KeyCode::KEY_F3            },
    { VK_F4              , EventKeyboard::KeyCode::KEY_F4            },
    { VK_F5              , EventKeyboard::KeyCode::KEY_F5            },
    { VK_F6              , EventKeyboard::KeyCode::KEY_F6            },
    { VK_F7              , EventKeyboard::KeyCode::KEY_F7            },
    { VK_F8              , EventKeyboard::KeyCode::KEY_F8            },
    { VK_F9              , EventKeyboard::KeyCode::KEY_F9            },
    { VK_F10             , EventKeyboard::KeyCode::KEY_F10           },
    { VK_F11             , EventKeyboard::KeyCode::KEY_F11           },
    { VK_F12             , EventKeyboard::KeyCode::KEY_F12           },
    { VK_F13             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F14             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F15             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F16             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F17             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F18             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F19             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F20             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F21             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F22             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F23             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F24             , EventKeyboard::KeyCode::KEY_NONE          },
    { VK_F24+1             , EventKeyboard::KeyCode::KEY_NONE          },
    { '0'            , EventKeyboard::KeyCode::KEY_0             },
    { '1'            , EventKeyboard::KeyCode::KEY_1             },
    { '2'            , EventKeyboard::KeyCode::KEY_2             },
    { '3'            , EventKeyboard::KeyCode::KEY_3             },
    { '4'            , EventKeyboard::KeyCode::KEY_4             },
    { '5'            , EventKeyboard::KeyCode::KEY_5             },
    { '6'            , EventKeyboard::KeyCode::KEY_6             },
    { '7'            , EventKeyboard::KeyCode::KEY_7             },
    { '8'            , EventKeyboard::KeyCode::KEY_8             },
    { '9'            , EventKeyboard::KeyCode::KEY_9             },
    { VK_DECIMAL      , EventKeyboard::KeyCode::KEY_PERIOD        },
    { VK_DIVIDE       , EventKeyboard::KeyCode::KEY_KP_DIVIDE     },
    { VK_MULTIPLY     , EventKeyboard::KeyCode::KEY_KP_MULTIPLY   },
    { VK_SUBTRACT     , EventKeyboard::KeyCode::KEY_KP_MINUS      },
    { VK_ADD          , EventKeyboard::KeyCode::KEY_KP_PLUS       },
    { GLFW_KEY_KP_ENTER        , EventKeyboard::KeyCode::KEY_KP_ENTER      },
    { GLFW_KEY_KP_EQUAL        , EventKeyboard::KeyCode::KEY_EQUAL         },
    { GLFW_KEY_LEFT_SHIFT      , EventKeyboard::KeyCode::KEY_SHIFT         },
    { GLFW_KEY_LEFT_CONTROL    , EventKeyboard::KeyCode::KEY_CTRL          },
    { GLFW_KEY_LEFT_ALT        , EventKeyboard::KeyCode::KEY_ALT           },
    { GLFW_KEY_LEFT_SUPER      , EventKeyboard::KeyCode::KEY_HYPER         },
    { VK_SHIFT     , EventKeyboard::KeyCode::KEY_SHIFT         },
    { VK_CONTROL   , EventKeyboard::KeyCode::KEY_CTRL          },
    { GLFW_KEY_RIGHT_ALT       , EventKeyboard::KeyCode::KEY_ALT           },
    { GLFW_KEY_RIGHT_SUPER     , EventKeyboard::KeyCode::KEY_HYPER         },
    { GLFW_KEY_MENU            , EventKeyboard::KeyCode::KEY_MENU          },
    { GLFW_KEY_LAST            , EventKeyboard::KeyCode::KEY_NONE          }
};


void InitEditorKeyCodeMap()
{
    if (g_EditorKeyCodeMap.empty())
    {
        for (auto& item : g_EditorKeyCodeStructArray)
        {
            g_EditorKeyCodeMap[item.windows_vk] = item.cocos_keycode;
        }
    }
}
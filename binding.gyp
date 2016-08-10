{
  'variables': {
      # may be redefined in command line on configuration stage
      'BUILD_LIBRDKAFKA%': '<!(echo ${BUILD_LIBRDKAFKA:-1})',
  },
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "<!@(ls -1 src/*.cc)", ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
      ],
      'conditions': [
        # If BUILD_LIBRDKAFKA, then
        # depend on librdkafka target, and use src-cpp when building
        # this target.
        [ '<(BUILD_LIBRDKAFKA)==1',
          {
            'dependencies': ['librdkafka'],
            'include_dirs': [ "deps/librdkafka/src-cpp" ],
            'libraries' : [ '-lz', '-lsasl2'],
          },
          # Else link against globally installed rdkafka and use
          # globally installed headers.  On Debian, you should
          # install the librdkafka1, librdkafka++1, and librdkafka-dev
          # .deb packages.
          {
            'libraries': ['-lz', '-lrdkafka', '-lrdkafka++'],
            'include_dirs': [
                "/usr/include/librdkafka",
                "/usr/local/include/librdkafka"
            ],
          },
        ],
        [ 'OS=="mac"',
          {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.11',
              'OTHER_CFLAGS' : ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers', '-std=c++11'],
            },
          }
        ],
        [ 'OS=="linux"',
          {
            'cflags': ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers'],
          }
        ]
      ]
    }
  ],
  "conditions": [
    [ '<(BUILD_LIBRDKAFKA)==1', {
        "targets": [
            {
              "target_name": "librdkafka_config_h",
              "type": "none",
              "actions": [
                {
                  'action_name': 'configure_librdkafka',
                  'message': 'configuring librdkafka...',
                  'inputs': [
                    'deps/librdkafka/configure',
                  ],
                  'outputs': [
                    'deps/librdkafka/config.h',
                  ],
                  'action': ['eval', 'cd deps/librdkafka && ./configure'],
                },
              ],
            },
            {
              "target_name": "librdkafka",
              "type": "static_library",
              'dependencies': [
                'librdkafka_config_h',
              ],
              "sources": [
                "<!@(ls -1 ./deps/librdkafka/src/*.c)",
                "<!@(ls -1 ./deps/librdkafka/src-cpp/*.cpp)",
              ],
              'cflags_cc!': [ '-fno-rtti' ],
              'conditions': [
                [
                  'OS=="mac"',
                  {
                    'xcode_settings': {
                      'MACOSX_DEPLOYMENT_TARGET': '10.11',
                      'OTHER_CFLAGS' : ['-Wno-sign-compare', '-Wno-missing-field-initializers'],
                      'GCC_ENABLE_CPP_RTTI': 'YES'
                    }
                  }
                ],[
                  'OS=="linux"',
                  {
                    'cflags' : [ '-Wno-sign-compare', '-Wno-missing-field-initializers', '-Wno-empty-body', '-g'],
                  }
                ]
              ]
            }
        ]

        }
    ]
    ]
}

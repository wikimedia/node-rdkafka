{
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "<!@(ls -1 src/*.cc)", ],
      'dependencies': [
        'librdkafka'
      ],
      "include_dirs": [
            "<!(node -e \"require('nan')\")",
            "deps/librdkafka/src-cpp"
      ],
      'conditions': [
        [ 'OS=="mac"', {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.11',
              'OTHER_CFLAGS' : ['-Wno-sign-compare', '-Wno-missing-field-initializers'],
            },
            'libraries' : ['-lz']
          }
        ]
      ]
    },
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
        "<!@(ls -1 deps/librdkafka/src-cpp/*.cpp)"
      ],
      'cflags_cc!': [ '-fno-rtti' ],
      'conditions': [
        [ 'OS=="mac"', {
            'xcode_settings': {
                'GCC_ENABLE_CPP_RTTI': 'YES',
                'MACOSX_DEPLOYMENT_TARGET': '10.11',
                'OTHER_CFLAGS' : ['-Wno-sign-compare', '-Wno-missing-field-initializers'],
            },
            'libraries' : ['-lz']
          }
        ]
      ]
    }
  ]
}
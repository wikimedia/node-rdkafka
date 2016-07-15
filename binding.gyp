{
  "targets": [
    {
      "target_name": "bindings",
      'dependencies': [
        'librdkafka'
      ],
      "sources": [ "<!@(ls -1 src/*.cc)", ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "deps/librdkafka/src-cpp"
      ],
      'conditions': [
        [ 'OS=="mac"',
          {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.11',
              'OTHER_CFLAGS' : ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers']
            },
            'libraries' : ['-lz']
          }
        ],
        [ 'OS=="linux"',
          {
            'cflags': ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers'],
            'libraries' : ['-lz']
          }
        ]
      ]
    },
    # Configure librdkafka
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
            'deps/librdkafka/Makefile.config',
          ],
          'action': ['eval', 'cd deps/librdkafka && ./configure'],
        },
      ],
    },
    # Build librdkafka
    {
      "target_name": "librdkafka",
      "type": "none",
      'dependencies': [
        'librdkafka_config_h',
      ],

      'conditions': [
        [ 'OS=="mac"',
          {
            'variables' : {
              'OUTPUTS': [
                'deps/librdkafka/src/librdkafka.1.dylib',
                'deps/librdkafka/src-cpp/librdkafka++.dylib'
              ],
            },
          },
        ],
        [ 'OS=="linux"',
          {
            'variables' : {
              'OUTPUTS': [
                'deps/librdkafka/src/librdkafka.so',
                'deps/librdkafka/src-cpp/librdkafka++.so',
              ],
            }
          }
        ]
      ],

      "actions": [
        {
          'action_name': 'make_librdkafka',
          'message': 'building librdkafka...',
          'inputs': [
            'deps/librdkafka/config.h',
            'deps/librdkafka/config.cache',
            'deps/librdkafka/Makefile.config',
          ],
          'outputs': [ '<@(OUTPUTS)' ],
          'action': ['eval', 'cd deps/librdkafka && make'],
        },
      ],
    }
  ]
}
